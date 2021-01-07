#include "instance.hpp"

#include <cstdlib>
#include <cassert>
#include <cmath>

#include "utils.hpp"


Instance::Instance(char *filename){
    FILE *arch = fopen(filename, "r");
    assert(arch != nullptr);

    readTrucks(arch);
    readMilk(arch);
    readNodes(arch);

    fclose(arch);

    assert(nodesAmount != 0);
    distanceBetweenNodes = (long double **)malloc(sizeof(long double *)*(nodesAmount+1));
    assert(distanceBetweenNodes != nullptr);
    for(unsigned long i = 1; i < nodesAmount+1; ++i){
        distanceBetweenNodes[i] = (long double *)malloc(sizeof(long double)*(nodesAmount+1));
        assert(distanceBetweenNodes[i] != nullptr);
        auto *node_i = nodesList.at(i-1).get();
        // Los nodos tienen distancia cero a si mismos.
        distanceBetweenNodes[i][i] = 0;

        for(unsigned long j = i+1; j < nodesAmount+1; ++j){
            long double dist = (*node_i).distanceTo(*nodesList.at(j-1).get());
            assert(!std::isnan(dist));
            assert(!std::isinf(dist));
            assert(std::isfinite(dist));
            distanceBetweenNodes[i][j] = dist;
        }

        (*node_i).setDistanceMatrix(distanceBetweenNodes);
    }
    // Calcula diagonal inferior.
    for(unsigned long i = 1+1; i < nodesAmount+1; ++i){
        for(unsigned long j = 1; j < i; ++j){
            distanceBetweenNodes[i][j] = distanceBetweenNodes[j][i];
        }
    }
}

Instance::~Instance(){
    for(unsigned long i = 1; i < nodesAmount+1; ++i){
        free(distanceBetweenNodes[i]);
    }
    free(distanceBetweenNodes);
}


void Instance::print(bool /*unused*/) const{
    for(const auto &truck: trucksList){
        truck.print(true);
    }
    printf("\n");

    milkList.print(true);

    for(const auto &node: nodesList){
        (*node).print(true);
    }
    printf("\n");

    printf("      ");
    for(unsigned long i = 1; i < nodesAmount+1; ++i){
        printf("%5lu ", i);
    }
    printf("\n");
    for(unsigned long i = 1; i < nodesAmount+1; ++i){
        printf("%5lu ", i);
        for(unsigned long j = 1; j < nodesAmount+1; ++j){
            printf("%5.1Lf ", distanceBetweenNodes[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}


Node *Instance::getInitialNode() const{
    return nodesList.at(0).get();
}


Solution Instance::initialSolution(){
    return Solution::initialSolution(nodesList, &milkList, trucksList, getInitialNode());
}


std::vector<long> Instance::getQuotasDiff(const std::vector<Route> &sol) const{
    std::vector<long> quotas_diff;
    quotas_diff.reserve(milkList.getTotal());

    for(const auto &milk: milkList){
        quotas_diff.push_back(milk.getMilkQuota());
    }
    for(const auto &route: sol){
        char milk_type = route.getMilkType();
        quotas_diff[milk_type - 'A'] -= route.getMilkAmount();
    }

    return quotas_diff;
}

bool Instance::didQuotasDiffImproved(const std::vector<long> &quotas_diff, const Solution &sol) const{
    for(const auto &route: sol.getRoutes()){
        char milk_type = route.getMilkType();
        long i = milk_type - 'A';
        if(quotas_diff[i] <= 0){
            // Solo interesa si mejoraron las infactibles.
            continue;
        }

        long diff = milkList.at(milk_type).getMilkQuota() - route.getMilkAmount();
        if(diff < quotas_diff[i]){
            return true;
        }
    }
    return false;
}

bool Instance::didCapacitiesLeftImproved(const Solution &original_solution, long src_route_index, const Route &src_route, const Route &dst_route) const{
    if(dst_route.getCapacityLeft() < 0){
        return false;
    }
    if(original_solution.getRoutes().at(src_route_index).getCapacityLeft() < 0){
        if(src_route.getCapacityLeft() > original_solution.getRoutes().at(src_route_index).getCapacityLeft()){
            return true;
        }
    }

    return false;
}


Solution Instance::hillClimbing(const Solution &initial_solution, long K) const{
    Solution best_solution(initial_solution);
    Solution solution(initial_solution);

    if(Utils::debugPrintingEnabled) Utils::debugPrint("initial_quality: %Lf\n\n", best_solution.evaluateSolution());

    for(long i = 0; i < K; ++i){
        bool is_better_solution = false;
        is_better_solution |= movement_extraLocalSearch(solution);
        is_better_solution |= movement_intraLocalSearch(solution);
        is_better_solution |= movement_removeOneNode(solution);

        if(Utils::debugPrintingEnabled) Utils::debugPrint("i: %5li - quality: %Lf\n\n", i, solution.evaluateSolution());
        if(is_better_solution){
            best_solution = solution;
        }
        else{
            break;
        }
    }
    Utils::debugPrint("\n");

    return best_solution;
}


// Mover un nodo de una ruta a las demás rutas.
bool Instance::movement_extraLocalSearch(Solution &solution) const{
    long double old_quality = solution.evaluateSolution();
    Solution alternative(solution);

    const std::vector<Route> &routes_list = alternative.getRoutes();

    // Randomizar el orden en que se seleccionarán las rutas de origen (src_route),
    // sin tener que randomizar el vector original.
    std::vector<long> order_of_selected_routes_src(Utils::range(routes_list.size()));
    Utils::randomizeVector(order_of_selected_routes_src);

    for(const long &src_route_index: order_of_selected_routes_src){
        //Route &src_route = alternative.at(src_route_index);

        // Randomizar el orden en que se seleccionarán las rutas de destino (dst_route),
        // sin tener que randomizar el vector original.
        std::vector<long> order_of_selected_routes_dst(Utils::range(routes_list.size()));
        Utils::randomizeVector(order_of_selected_routes_dst);

        for(const long &dst_route_index: order_of_selected_routes_dst){
            // Verificar que la ruta destino no sea la misma que la ruta origen.
            if(src_route_index == dst_route_index){
                continue;
            }

            //Route &dst_route = alternative.at(dst_route_index);

            bool did_quality_improved = tryMoveNodeBetweenRoutes(solution, alternative, old_quality, src_route_index, dst_route_index);
            if(did_quality_improved){
                solution = alternative;
                return true;
            }
        }
    }

    // Ya se probaron todas las vecindades posibles y ninguna dio una solución de mejor calidad.
    return false;
}

// Movimiento 2-opt de la ruta consigo misma.
bool Instance::movement_intraLocalSearch(Solution &solution) const{
    long double old_quality = solution.evaluateSolution();
    Solution alternative(solution);

    const std::vector<Route> &routes_list = alternative.getRoutes();

    // Randomizar el orden en que se seleccionarán las rutas (route);
    // sin tener que randomizar el vector original.
    std::vector<long> order_of_selected_routes(Utils::range(routes_list.size()));
    Utils::randomizeVector(order_of_selected_routes);

    for(const long &route_index: order_of_selected_routes){
        //Route &route = alternative.at(route_index);

        bool did_quality_improved = try2OptInRoute(alternative, old_quality, route_index);
        if(did_quality_improved){
            solution = alternative;
            return true;
        }
    }

    // Ya se probaron todas las vecindades posibles y ninguna dio una solución de mejor calidad.
    return false;
}

bool Instance::movement_removeOneNode(Solution &solution) const{
    long double old_quality = solution.evaluateSolution();
    Solution alternative(solution);

    const std::vector<Route> &routes_list = alternative.getRoutes();

    // Randomizar el orden en que se seleccionarán las rutas (route);
    // sin tener que randomizar el vector original.
    std::vector<long> order_of_selected_routes(Utils::range(routes_list.size()));
    Utils::randomizeVector(order_of_selected_routes);

    for(const long &route_index: order_of_selected_routes){
        //Route &route = routes_list.at(route_index);

        const std::vector<const Node *> &nodes_list = routes_list.at(route_index).getNodes();

        // Randomizar el orden en que se borrarán los nodos sin tener que randomizar el vector original.
        std::vector<long> order_of_selected_nodes(Utils::range(nodes_list.size()));
        Utils::randomizeVector(order_of_selected_nodes);

        for(const long &pos: order_of_selected_nodes){
            const Node *node = nodes_list.at(pos);
            bool removed_without_problems = alternative.removeFarmFromRoute(route_index, pos);
            if(removed_without_problems){
                long double new_quality = alternative.evaluateSolution();
                if(new_quality > old_quality){
                    Utils::debugPrint("remove: improve quality\n");
                    solution = alternative;
                    return true;
                }
            }
            alternative.addFarmToRoute(route_index, pos, node);
        }
    }

    // Ya se probaron todas las vecindades posibles y ninguna dio una solución de mejor calidad.
    return false;
}


bool Instance::tryMoveNodeBetweenRoutes(const Solution &original_solution, Solution &alternative, long double old_quality, long src_route_index, long dst_route_index) const{
    auto quotas_diff(getQuotasDiff(alternative.getRoutes()));
    bool was_feasible = alternative.isFeasible();

    const Route &src_route = alternative.getRoutes().at(src_route_index);
    const Route &dst_route = alternative.getRoutes().at(dst_route_index);

    const std::vector<const Node *> &nodes_list_src = alternative.getRoutes().at(src_route_index).getNodes();

    // Randomizar el orden en que se seleccionarán los nodos de la ruta de origen (node_src),
    // sin tener que randomizar el vector original.
    std::vector<long> order_of_selected_nodes_src(Utils::range(nodes_list_src.size()));
    Utils::randomizeVector(order_of_selected_nodes_src);

    for(const long &src_nodes_index: order_of_selected_nodes_src){
        const Node *node_src = nodes_list_src.at(src_nodes_index);

        // Evitar agregar el nodo a la ruta si este sobrecarga al camión.
        if(dst_route.getCapacityLeft() - (*node_src).getProduced() < 0){
            continue;
        }

        // Se saca el nodo de la ruta de la cual proviene.
        bool removed_without_problems = alternative.removeFarmFromRoute(src_route_index, src_nodes_index);
        if(removed_without_problems){
            const std::vector<const Node *> &nodes_list_dst = dst_route.getNodes();

            // Randomizar el orden en que se insertarán los nodos en la ruta de destino,
            // sin tener que randomizar el vector original.
            std::vector<long> order_of_selected_nodes_dst(Utils::range(nodes_list_dst.size()));
            Utils::randomizeVector(order_of_selected_nodes_dst);

            for(const long &dst_nodes_index: order_of_selected_nodes_dst){
                // Se agrega el nodo a la ruta de destino en la posición correspondiente
                // y calcular la calidad de esta nueva solución.
                bool added_without_problems = alternative.addFarmToRoute(dst_route_index, dst_nodes_index, node_src);

                if(added_without_problems){
                    if(!was_feasible){
                        bool capacities_improved = didCapacitiesLeftImproved(original_solution, src_route_index, src_route, dst_route);
                        bool quotas_improved = didQuotasDiffImproved(quotas_diff, alternative);
                        if(capacities_improved || quotas_improved){
                            Utils::debugPrint("Move: improve feasibility\n");
                            Utils::debugPrint("\tNode: %ld\n\tTruck src: %ld\n\tTruckdst: %ld\n", (*node_src).getId(), src_route.getTruckId(), dst_route.getTruckId());
                            return true;
                        }
                    }
                    else{
                        // Si la calidad es mejor, actualizamos la solución y retornamos (Alguna mejora).
                        long double new_quality = alternative.evaluateSolution();
                        if(new_quality > old_quality){
                            Utils::debugPrint("Move: improve quality\n");
                            Utils::debugPrint("\tNode: %ld\n\tTruck src: %ld\n\tTruckdst: %ld\n", (*node_src).getId(), src_route.getTruckId(), dst_route.getTruckId());
                            return true;
                        }
                    }
                }
                // Si la nueva calidad no supera la calidad anterior, quitamos el nodo de
                // esa posición y reintentamos en la siguiente posición.
                alternative.removeFarmFromRoute(dst_route_index, dst_nodes_index);
            }
        }

        // Insertar este nodo en esta ruta destino no dio mejoras, por lo que recolocamos
        // el nodo en su ruta original, y en su posición original.
        alternative.addFarmToRoute(src_route_index, src_nodes_index, node_src);
    }

    return false;
}

bool Instance::try2OptInRoute(Solution &alternative, long double old_quality, long route_index) const{
    const std::vector<const Node *> &nodes_list = alternative.getRoutes().at(route_index).getNodes();

    // Randomizar el orden de los nodos iniciales con los que se realizará el 2-opt (pos_left),
    // sin tener que randomizar el vector original.
    std::vector<long> order_of_selected_nodes_left(Utils::range(nodes_list.size()));
    Utils::randomizeVector(order_of_selected_nodes_left);

    for(const long &pos_left: order_of_selected_nodes_left){
        // Randomizar el orden de los nodos finales con los que se realizará el 2-opt (pos_right),
        // sin tener que randomizar el vector original.
        // El nodo final siempre estará "más a la derecha" que el nodo inicial.
        std::vector<long> order_of_selected_nodes_right(Utils::range(pos_left+1, nodes_list.size()));
        Utils::randomizeVector(order_of_selected_nodes_right);

        for(const long &pos_right: order_of_selected_nodes_right){
            // Se realiza el 2-opt en el rango seleccionado. Si la solución es de mejor calidad
            // se actualiza la solución y se retorna (Alguna mejora).
            alternative.reverseFarmsOrderInRoute(route_index, pos_left, pos_right);
            long double new_quality = alternative.evaluateSolution();
            if(new_quality > old_quality){
                Utils::debugPrint("2opt: improve quality\n");
                return true;
            }

            // En caso de que la solución no sea de mejor calidad, se deshace el 2-opt
            // y se prueba con un rango distinto.
            alternative.reverseFarmsOrderInRoute(route_index, pos_left, pos_right);
        }
    }

    return false;
}


void Instance::readTrucks(FILE *arch){
    fscanf(arch, "%lu", &trucksAmount);
    assert(trucksAmount != 0);

    trucksList.reserve(trucksAmount);
    for(unsigned long i = 0; i < trucksAmount; ++i){
        long capacity;
        fscanf(arch, "%ld", &capacity);
        trucksList.emplace_back(i+1, capacity);
    }
}

void Instance::readMilk(FILE *arch){
    unsigned long milk_types_amount;
    fscanf(arch, "%lu", &milk_types_amount);
    assert(milk_types_amount != 0);

    std::vector<long> quotas_list;
    quotas_list.reserve(milk_types_amount);
    for(unsigned long i = 0; i < milk_types_amount; ++i){
        long data;
        fscanf(arch, "%ld", &data);
        quotas_list.emplace_back(data);
    }

    std::vector<long double> profit_list;
    profit_list.reserve(milk_types_amount);
    for(unsigned long i = 0; i < milk_types_amount; ++i){
        long double data;
        fscanf(arch, "%Lf", &data);
        profit_list.emplace_back(data);
    }

    milkList.setTotal(milk_types_amount);
    for(unsigned long i = 0; i < milk_types_amount; ++i){
        milkList.add(static_cast<char>(i), quotas_list.at(i), profit_list.at(i));
    }
}

void Instance::readNodes(FILE *arch){
    fscanf(arch, "%lu", &nodesAmount);
    assert(nodesAmount != 0);

    nodesList.reserve(nodesAmount);
    for(unsigned long i = 0; i < nodesAmount; ++i){
        long id, x, y, amountProduced;
        char typeProduced;
        fscanf(arch, "%ld %ld %ld %c %ld", &id, &x, &y, &typeProduced, &amountProduced);
        nodesList.push_back(std::make_unique<Node>(id, x, y, typeProduced, amountProduced));
        if(typeProduced >= 'A') milkList.at(typeProduced).addProducedMilk(amountProduced);
    }
}
