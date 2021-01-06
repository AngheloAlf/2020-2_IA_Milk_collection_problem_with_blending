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


const Node *Instance::getInitialNode() const{
    return nodesList.at(0).get();
}


std::vector<Route> Instance::initialSolution() const{
    std::vector<Route> routes;
    routes.reserve(trucksAmount);
    char milk_type = 'A';
    for(unsigned long i = 0; i < trucksAmount; ++i){
        routes.emplace_back(&milkList, milk_type++, nodesAmount, milkList.getTotal());
    }

    bool valid = false;
    while(!valid){
        valid = true;
        Utils::randomizeVector(routes);

        for(unsigned long i = 0; valid && i < trucksAmount; ++i){
            const auto &truck = trucksList.at(i);
            auto &route = routes.at(i);
            const auto milk_info = route.getMilkTypeInfo();
            if(truck.getCapacity() < milk_info.getMilkQuota()){
                valid = false;
                break;
            }

            route.setTruck(truck);
        }
    }

    std::vector<Node *> farms_list;
    farms_list.reserve(nodesList.size());
    for(auto iter = nodesList.begin()+1; iter < nodesList.end(); ++iter){
        farms_list.push_back((*iter).get());
    }
    Utils::randomizeVector(farms_list);

    long TOL = 0;
    const long tol_change = 10;
    long truck_counter = 0;

    /*print(true);

    for(auto &asdf: farms_list){
        asdf.print(true);
    }*/

    while(!farms_list.empty()){
        auto selected_farm_iter = Utils::selectRandomly(farms_list);
        assert(selected_farm_iter != farms_list.end());
        Node *selected_farm = *selected_farm_iter;

        for(unsigned long i = 0; i < trucksList.size(); ++i){
            Route &route = routes.at(i);
            if(route.getCapacityLeft() - (*selected_farm).getProduced() >= TOL && route.getMilkType() == (*selected_farm).getQuality()){
                route.addFarm(selected_farm);
                farms_list.erase(selected_farm_iter);
                ++truck_counter;

                if(farms_list.empty()) break;

                selected_farm_iter = Utils::selectRandomly(farms_list);
                assert(selected_farm_iter != farms_list.end());
                selected_farm = *selected_farm_iter;
            }
        }

        if(truck_counter == 0){
            TOL -= tol_change;
            continue;
        }
        truck_counter = 0;
    }

    return routes;
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

bool Instance::didQuotasDiffImproved(const std::vector<long> &quotas_diff, const std::vector<Route> &sol) const{
    for(const auto &route: sol){
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

bool Instance::didCapacitiesLeftImproved(const std::vector<Route> &original_solution, long src_route_index, const Route &src_route, const Route &dst_route) const{
    if(dst_route.getCapacityLeft() < 0){
        return false;
    }
    if(original_solution.at(src_route_index).getCapacityLeft() < 0){
        if(src_route.getCapacityLeft() > original_solution.at(src_route_index).getCapacityLeft()){
            return true;
        }
    }

    return false;
}

bool Instance::isFeasible(const std::vector<Route> &sol) const{
    static std::vector<long> quotas_aux;
    quotas_aux.reserve(milkList.getTotal());
    quotas_aux.clear();

    for(const auto &milk: milkList){
        quotas_aux.push_back(milk.getMilkQuota());
    }
    for(const auto &route: sol){
        if(!route.isFeasible()){
            return false;
        }
        char milk_type = route.getMilkType();
        quotas_aux[milk_type - 'A'] -= route.getMilkAmount();
    }
    return !std::any_of(quotas_aux.begin(), quotas_aux.end(), [](const auto &q){ return q > 0; });
}


long double Instance::evaluateSolution(std::vector<Route> &sol) const{
    long double result = 0;

    /*// Verificar que todas las cuota de la planta procesadora se cumplan.
    static std::vector<long> quotas_aux;
    quotas_aux.reserve(milkList.getTotal());
    quotas_aux.clear();
    for(const auto &milk: milkList){
        quotas_aux.push_back(milk.getMilkQuota());
    }
    for(const auto &route: sol){
        char milk_type = route.getMilkType();
        quotas_aux[milk_type - 'A'] -= route.getMilkAmount();
    }
    for(const auto &q: quotas_aux){
        if(q > 0){
            // Restricción: El total de la leche de cada tipo (haya sido mezclada o no) debe superar la cuota mínima de la planta procesadora.
            return 0;
        }
    }*/

    const auto *initial_node = getInitialNode();
    for(Route &route: sol){
        result += route.evaluateRoute(initial_node);
    }

    return result;
}
long double Instance::calculateTransportCosts(const std::vector<Route> &sol) const{
    long double result = 0;

    const auto *initial_node = getInitialNode();
    for(const Route &route: sol){
        result += route.calculateTransportCosts(initial_node);
    }

    return result;
}
long double Instance::calculateMilkProfits(const std::vector<Route> &sol) const{
    long double result = 0;
    for(const Route &route: sol){
        result += route.calculateMilkProfits();
    }
    return result;
}


std::vector<Route> Instance::hillClimbing(const std::vector<Route> &initial_solution, long K) const{
    std::vector<Route> best_solution(initial_solution);
    std::vector<Route> solution(initial_solution);
    long double best_quality = evaluateSolution(best_solution);

    Utils::debugPrint("initial_quality: %Lf\n\n", best_quality);

    for(long i = 0; i < K; ++i){
        bool is_better_solution = false;
        is_better_solution |= extraLocalSearch(solution);
        is_better_solution |= intraLocalSearch(solution);

        Utils::debugPrint("i: %5li - quality: %Lf\n\n", i, evaluateSolution(solution));
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
bool Instance::extraLocalSearch(std::vector<Route> &solution) const{
    long double old_quality = evaluateSolution(solution);
    std::vector<Route> alternative(solution);

    // Randomizar el orden en que se seleccionarán las rutas de origen (src_route),
    // sin tener que randomizar el vector original.
    std::vector<long> order_of_selected_routes_src(Utils::range(alternative.size()));
    Utils::randomizeVector(order_of_selected_routes_src);

    for(const long &src_route_index: order_of_selected_routes_src){
        Route &src_route = alternative.at(src_route_index);

        // Randomizar el orden en que se seleccionarán las rutas de destino (dst_route),
        // sin tener que randomizar el vector original.
        std::vector<long> order_of_selected_routes_dst(Utils::range(alternative.size()));
        Utils::randomizeVector(order_of_selected_routes_dst);

        for(const long &dst_route_index: order_of_selected_routes_dst){
            // Verificar que la ruta destino no sea la misma que la ruta origen.
            if(src_route_index == dst_route_index){
                continue;
            }

            Route &dst_route = alternative.at(dst_route_index);

            bool did_quality_improved = tryMoveNodeBetweenRoutes(solution, alternative, old_quality, src_route_index, src_route, dst_route);
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
bool Instance::intraLocalSearch(std::vector<Route> &solution) const{
    long double old_quality = evaluateSolution(solution);
    std::vector<Route> alternative(solution);

    // Randomizar el orden en que se seleccionarán las rutas (route);
    // sin tener que randomizar el vector original.
    std::vector<long> order_of_selected_routes(Utils::range(alternative.size()));
    Utils::randomizeVector(order_of_selected_routes);

    for(const long &route_index: order_of_selected_routes){
        Route &route = alternative.at(route_index);

        bool did_quality_improved = try2OptInRoute(alternative, old_quality, route);
        if(did_quality_improved){
            solution = alternative;
            return true;
        }
    }

    // Ya se probaron todas las vecindades posibles y ninguna dio una solución de mejor calidad.
    return false;
}


bool Instance::tryMoveNodeBetweenRoutes(const std::vector<Route> &original_solution, std::vector<Route> &alternative, long double old_quality, long src_route_index, Route &src_route, Route &dst_route) const{
    auto quotas_diff(getQuotasDiff(alternative));
    bool was_feasible = isFeasible(alternative);

    const std::vector<const Node *> &nodes_list_src = src_route.getNodes();

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
        bool removed_without_problems = src_route.removeFarm(src_nodes_index);
        if(removed_without_problems){
            const std::vector<const Node *> &nodes_list_dst = dst_route.getNodes();

            // Randomizar el orden en que se insertarán los nodos en la ruta de destino,
            // sin tener que randomizar el vector original.
            std::vector<long> order_of_selected_nodes_dst(Utils::range(nodes_list_dst.size()));
            Utils::randomizeVector(order_of_selected_nodes_dst);

            for(const long &dst_nodes_index: order_of_selected_nodes_dst){
                // Se agrega el nodo a la ruta de destino en la posición correspondiente
                // y calcular la calidad de esta nueva solución.
                bool added_without_problems = dst_route.addFarm(dst_nodes_index, node_src);

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
                        long double new_quality = evaluateSolution(alternative);
                        if(new_quality > old_quality){
                            Utils::debugPrint("Move: improve quality\n");
                            Utils::debugPrint("\tNode: %ld\n\tTruck src: %ld\n\tTruckdst: %ld\n", (*node_src).getId(), src_route.getTruckId(), dst_route.getTruckId());
                            return true;
                        }
                    }
                }
                // Si la nueva calidad no supera la calidad anterior, quitamos el nodo de
                // esa posición y reintentamos en la siguiente posición.
                dst_route.removeFarm(dst_nodes_index);
            }
        }

        // Insertar este nodo en esta ruta destino no dio mejoras, por lo que recolocamos
        // el nodo en su ruta original, y en su posición original.
        src_route.addFarm(src_nodes_index, node_src);
    }

    return false;
}

bool Instance::try2OptInRoute(std::vector<Route> &alternative, long double old_quality, Route &route) const{
    const std::vector<const Node *> &nodes_list = route.getNodes();

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
            route.reverseFarmsOrder(pos_left, pos_right);
            long double new_quality = evaluateSolution(alternative);
            if(new_quality > old_quality){
                Utils::debugPrint("2opt: improve quality\n");
                return true;
            }

            // En caso de que la solución no sea de mejor calidad, se deshace el 2-opt
            // y se prueba con un rango distinto.
            route.reverseFarmsOrder(pos_left, pos_right);
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
