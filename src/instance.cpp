#include "instance.hpp"

#include <cstdlib>
#include <cassert>
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

        for(unsigned long j = i+1+1; j < nodesAmount+1; ++j){
            long double dist = (*node_i).distanceTo(*nodesList.at(j-1).get());
            distanceBetweenNodes[i][j] = dist;
        }

        (*node_i).setDistanceMatrix(distanceBetweenNodes);
    }
    // Calcula diagonal inferior.
    for(unsigned long i = 1+1; i < nodesAmount+1; ++i){
        for(unsigned long j = 0; j < i; ++j){
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

    for(const auto &milk: milkList){
        milk.print(true);
    }
    printf("\n");

    for(const auto &node: nodesList){
        (*node).print(true);
    }
    printf("\n");

    for(unsigned long i = 0; i < nodesAmount; ++i){
        for(unsigned long j = 0; j < nodesAmount; ++j){
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
        routes.emplace_back(milk_type++, nodesAmount, milkTypesAmount);
    }
    Utils::randomizeVector(routes);
    for(unsigned long i = 0; i < trucksAmount; ++i){
        routes.at(i).setTruck(trucksList.at(i));
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

long double Instance::evaluateSolution(std::vector<Route> &sol) const{
    long double result = 0;

    const auto *initial_node = getInitialNode();
    for(Route &route: sol){
        result += route.evaluateRoute(initial_node, milkList);
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
        result += route.calculateMilkProfits(milkList);
    }
    return result;
}


std::vector<Route> Instance::hillClimbing(const std::vector<Route> &initial_solution, long K) const{
    std::vector<Route> best_solution(initial_solution);
    std::vector<Route> solution(initial_solution);
    long double best_quality = evaluateSolution(best_solution);

    Utils::debugPrint("initial_quality: %Lf\n", best_quality);

    bool global_local = false;
    for(long i = 0; i < K && !global_local; ++i){
        global_local = true;
        global_local &= extraLocalSearch(solution);
        global_local &= intraLocalSearch(solution);

        long double quality = evaluateSolution(solution);
        Utils::debugPrint("i: %5li - quality: %Lf\n", i, quality);
        if(quality > best_quality){
            best_solution = solution;
            best_quality = quality;
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

            bool did_quality_improved = tryMoveNodeBetweenRoutes(alternative, old_quality, src_route, dst_route);
            if(did_quality_improved){
                solution = alternative;
                return false;
            }
        }
    }

    // Ya se probaron todas las vecindades posibles y ninguna dio una solución de mejor calidad.
    return true;
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
            return false;
        }
    }

    // Ya se probaron todas las vecindades posibles y ninguna dio una solución de mejor calidad.
    return true;
}


bool Instance::tryMoveNodeBetweenRoutes(std::vector<Route> &alternative, long double old_quality, Route &src_route, Route &dst_route) const{
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
        src_route.removeFarm(src_nodes_index);

        const std::vector<const Node *> &nodes_list_dst = dst_route.getNodes();

        // Randomizar el orden en que se insertarán los nodos en la ruta de destino,
        // sin tener que randomizar el vector original.
        std::vector<long> order_of_selected_nodes_dst(Utils::range(nodes_list_dst.size()));
        Utils::randomizeVector(order_of_selected_nodes_dst);

        for(const long &dst_nodes_index: order_of_selected_nodes_dst){
            // Se agrega el nodo a la ruta de destino en la posición correspondiente
            // y calcular la calidad de esta nueva solución.
            // Si la calidad es mejor, actualizamos la solución y retornamos (Alguna mejora).
            dst_route.addFarm(dst_nodes_index, node_src);
            long double new_quality = evaluateSolution(alternative);
            if(new_quality > old_quality){
                return true;
            }

            // Si la nueva calidad no supera la calidad anterior, quitamos el nodo de
            // esa posición y reintentamos en la siguiente posición.
            dst_route.removeFarm(dst_nodes_index);
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
    fscanf(arch, "%lu", &milkTypesAmount);
    assert(milkTypesAmount != 0);

    std::vector<long> quotas_list;
    quotas_list.reserve(milkTypesAmount);
    for(unsigned long i = 0; i < milkTypesAmount; ++i){
        long data;
        fscanf(arch, "%ld", &data);
        quotas_list.emplace_back(data);
    }

    std::vector<long double> profit_list;
    profit_list.reserve(milkTypesAmount);
    for(unsigned long i = 0; i < milkTypesAmount; ++i){
        long double data;
        fscanf(arch, "%Lf", &data);
        profit_list.emplace_back(data);
    }

    char milk_type = 'A';
    milkList.reserve(milkTypesAmount);
    for(unsigned long i = 0; i < milkTypesAmount; ++i){
        milkList.emplace_back(milk_type, quotas_list.at(i), profit_list.at(i));
        ++milk_type;
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
    }
}

