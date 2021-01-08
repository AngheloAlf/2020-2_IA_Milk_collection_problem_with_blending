#include "solution.hpp"

#include <cassert>

#include "utils.hpp"


Solution::Solution(const Instance *instance)
: instance(instance){
}

Solution Solution::initialSolution(const Instance *instance){
    Solution sol(instance);
    sol.routes.reserve((*instance).getTrucksList().size());

    char milk_type = 'A';
    for(unsigned long i = 0; i < (*instance).getTrucksList().size(); ++i){
        sol.routes.emplace_back(&(*instance).getMilkList(), milk_type++, (*instance).getNodesList().size(), (*instance).getMilkList().getTotal());
    }

    bool valid = false;
    while(!valid){
        valid = true;
        Utils::randomizeVector(sol.routes);

        for(unsigned long i = 0; valid && i < (*instance).getTrucksList().size(); ++i){
            const auto &truck = (*instance).getTrucksList().at(i);
            auto &route = sol.routes.at(i);
            const auto milk_info = route.getMilkTypeInfo();
            if(truck.getCapacity() < milk_info.getMilkQuota()){
                valid = false;
                break;
            }

            route.setTruck(truck);
        }
    }

    std::vector<Node *> farms_list;
    farms_list.reserve((*instance).getNodesList().size());
    for(auto iter = (*instance).getNodesList().begin()+1; iter < (*instance).getNodesList().end(); ++iter){
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

        for(unsigned long i = 0; i < (*instance).getTrucksList().size(); ++i){
            Route &route = sol.routes.at(i);
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

    return sol;
}

void Solution::print(bool newline) const{
    for(unsigned long i = 0; i < routes.size(); ++i){
        printf("%li: ", i);
        const auto &route = routes.at(i);
        route.print(true);
    }
    printf("Value: %Lf\n%s", calculateMilkProfits()-calculateTransportCosts(), newline?"\n":"");
}


const std::vector<Route> & Solution::getRoutes() const{
    return routes;
}

bool Solution::isFeasible() const{
    static std::vector<long> quotas_aux;
    quotas_aux.reserve((*instance).getMilkList().getTotal());
    quotas_aux.clear();

    for(const auto &milk: (*instance).getMilkList()){
        quotas_aux.push_back(milk.getMilkQuota());
    }
    for(const auto &route: routes){
        if(!route.isFeasible()){
            return false;
        }
        char milk_type = route.getMilkType();
        quotas_aux[milk_type - 'A'] -= route.getMilkAmount();
    }
    return !std::any_of(quotas_aux.begin(), quotas_aux.end(), [](const auto &q){ return q > 0; });
}

bool Solution::canAddFarmToRoute(long route_index, const Node *farm) const{
    return routes.at(route_index).canAddFarm(farm);
}
bool Solution::canRemoveFarmFromRoute(long route_index, long position) const{
    return routes.at(route_index).canRemoveFarm(position);
}

void Solution::addFarmToRoute(long route_index, long position, const Node *farm){
    routes.at(route_index).addFarm(position, farm);
}
void Solution::removeFarmFromRoute(long route_index, long position){
    routes.at(route_index).removeFarm(position);
}
void Solution::reverseFarmsOrderInRoute(long route_index, long left, long right){
    routes.at(route_index).reverseFarmsOrder(left, right);
}
void Solution::setFarmInRoute(long route_index, long position, const Node *farm){
    routes.at(route_index).setFarm(position, farm);
}


std::vector<long> Solution::getQuotasDiff() const{
    std::vector<long> quotas_diff;
    quotas_diff.reserve((*instance).getMilkList().getTotal());

    for(const auto &milk: (*instance).getMilkList()){
        quotas_diff.push_back(milk.getMilkQuota());
    }
    for(const auto &route: routes){
        char milk_type = route.getMilkType();
        quotas_diff[milk_type - 'A'] -= route.getMilkAmount();
    }

    return quotas_diff;
}

bool Solution::didQuotasDiffImproved(const std::vector<long> &quotas_diff) const{
    for(const auto &route: routes){
        char milk_type = route.getMilkType();
        long i = milk_type - 'A';
        if(quotas_diff[i] <= 0){
            // Solo interesa si mejoraron las infactibles.
            continue;
        }

        long diff = (*instance).getMilkList().at(milk_type).getMilkQuota() - route.getMilkAmount();
        if(diff < quotas_diff[i]){
            return true;
        }
    }
    return false;
}

bool Solution::didCapacitiesLeftImproved(long old_capacity_left, long new_capacity_left) const{
    if(old_capacity_left < 0){
        if(new_capacity_left > old_capacity_left){
            return true;
        }
    }
    return false;
}
bool Solution::didCapacitiesLeftImproved(long src_old_capacity_left, long src_new_capacity_left, long dst_old_capacity_left, long dst_new_capacity_left) const{
    if(src_old_capacity_left < 0){
        if(src_new_capacity_left > src_old_capacity_left){
            if(dst_new_capacity_left >= 0){
                return true;
            }
        }
    }
    if(dst_old_capacity_left < 0){
        if(dst_new_capacity_left > dst_old_capacity_left){
            if(src_new_capacity_left >= 0){
                return true;
            }
        }
    }
    return false;
}
bool Solution::didCapacitiesLeftImproved(long old_capacity_left, long new_capacity_left, long dst_new_capacity_left) const{
    if(dst_new_capacity_left < 0){
        return false;
    }
    return didCapacitiesLeftImproved(old_capacity_left, new_capacity_left);
}


long double Solution::evaluateSolution(){
    long double result = 0;

    for(Route &route: routes){
        result += route.evaluateRoute((*instance).getInitialNode());
    }

    return result;
}
long double Solution::calculateTransportCosts() const{
    long double result = 0;

    for(const Route &route: routes){
        result += route.calculateTransportCosts((*instance).getInitialNode());
    }

    return result;
}
long double Solution::calculateMilkProfits() const{
    long double result = 0;
    for(const Route &route: routes){
        result += route.calculateMilkProfits();
    }
    return result;
}


Solution Solution::hillClimbing(long K) const{
    Solution solution(*this);

    if(Utils::debugPrintingEnabled) Utils::debugPrint("initial_quality: %Lf\n\n", solution.evaluateSolution());

    std::vector<long> order_of_movements(Utils::range(4));

    long i = 0;
    for(; i < K; ++i){
        bool is_better_solution = false;
        Utils::randomizeVector(order_of_movements);
        for(auto &movement: order_of_movements){
            switch (movement){
            case 0:
                is_better_solution |= solution.movement_moveNodeBetweenRoutes();
                break;
            case 1:
                is_better_solution |= solution.movement_2OptIntraRoute();
                break;
            case 2:
                is_better_solution |= solution.movement_removeOneNode();
                break;
            case 3:
                is_better_solution |= solution.movement_interchangeNodesBetweenRoutes();
                break;
            }
        }

        if(Utils::debugPrintingEnabled) Utils::debugPrint("i: %5li - quality: %Lf\n\n", i, solution.evaluateSolution());
        if(!is_better_solution){
            break;
        }
    }
    Utils::debugPrint("\n");

    return solution;
}


// Mover un nodo de una ruta a las demás rutas.
bool Solution::movement_moveNodeBetweenRoutes(){
    long double old_quality = evaluateSolution();
    auto quotas_diff(getQuotasDiff());
    bool was_feasible = isFeasible();

    std::vector<long> order_of_selected_routes_src(Utils::range(routes.size()));
    std::vector<long> order_of_selected_routes_dst(Utils::range(routes.size()));

    // Randomizar el orden en que se seleccionarán las rutas de origen (src_route),
    // sin tener que randomizar el vector original.
    Utils::randomizeVector(order_of_selected_routes_src);
    for(const long &src_route_index: order_of_selected_routes_src){
        const Route &src_route = routes.at(src_route_index);
        const std::vector<const Node *> &nodes_list_src = src_route.getNodes();
        long src_old_capacity_left = src_route.getCapacityLeft();

        // Randomizar el orden en que se seleccionarán las rutas de destino (dst_route),
        // sin tener que randomizar el vector original.
        Utils::randomizeVector(order_of_selected_routes_dst);
        for(const long &dst_route_index: order_of_selected_routes_dst){
            // Verificar que la ruta destino no sea la misma que la ruta origen.
            if(src_route_index == dst_route_index){
                continue;
            }

            const Route &dst_route = routes.at(dst_route_index);

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
                bool can_be_removed_without_problems = canRemoveFarmFromRoute(src_route_index, src_nodes_index);
                if(can_be_removed_without_problems){
                    removeFarmFromRoute(src_route_index, src_nodes_index);
                    const std::vector<const Node *> &nodes_list_dst = dst_route.getNodes();

                    // Randomizar el orden en que se insertarán los nodos en la ruta de destino,
                    // sin tener que randomizar el vector original.
                    std::vector<long> order_of_selected_nodes_dst(Utils::range(nodes_list_dst.size()));
                    Utils::randomizeVector(order_of_selected_nodes_dst);
                    for(const long &dst_nodes_index: order_of_selected_nodes_dst){
                        bool can_be_added_without_problems = canAddFarmToRoute(dst_route_index, node_src);
                        if(!can_be_added_without_problems){
                            continue;
                        }

                        // Se agrega el nodo a la ruta de destino en la posición correspondiente
                        // y calcular la calidad de esta nueva solución.
                        addFarmToRoute(dst_route_index, dst_nodes_index, node_src);

                        if(!was_feasible){
                            bool capacities_improved = didCapacitiesLeftImproved(src_old_capacity_left, src_route.getCapacityLeft(), dst_route.getCapacityLeft());
                            bool quotas_improved = didQuotasDiffImproved(quotas_diff);
                            if(capacities_improved || quotas_improved){
                                Utils::debugPrint("Move: improve feasibility. ");
                                Utils::debugPrint("\tNode: %ld \tTruck src: %ld \tTruckdst: %ld\n", (*node_src).getId(), src_route.getTruckId(), dst_route.getTruckId());
                                return true;
                            }
                        }
                        // Si la calidad es mejor, actualizamos la solución y retornamos (Alguna mejora).
                        long double new_quality = evaluateSolution();
                        if(new_quality > old_quality){
                            Utils::debugPrint("Move: improve quality\n");
                            Utils::debugPrint("\tNode: %ld\n\tTruck src: %ld\n\tTruckdst: %ld\n", (*node_src).getId(), src_route.getTruckId(), dst_route.getTruckId());
                            return true;
                        }

                        // Si la nueva calidad no supera la calidad anterior, quitamos el nodo de
                        // esa posición y reintentamos en la siguiente posición.
                        removeFarmFromRoute(dst_route_index, dst_nodes_index);
                    }
                    // Insertar este nodo en esta ruta destino no dio mejoras, por lo que recolocamos
                    // el nodo en su ruta original, y en su posición original.
                    addFarmToRoute(src_route_index, src_nodes_index, node_src);
                }
            }
        }
    }

    // Ya se probaron todas las vecindades posibles y ninguna dio una solución de mejor calidad.
    return false;
}

// Movimiento 2-opt de la ruta consigo misma.
bool Solution::movement_2OptIntraRoute(){
    long double old_quality = evaluateSolution();

    // Randomizar el orden en que se seleccionarán las rutas (route);
    // sin tener que randomizar el vector original.
    std::vector<long> order_of_selected_routes(Utils::range(routes.size()));
    Utils::randomizeVector(order_of_selected_routes);

    for(const long &route_index: order_of_selected_routes){
        const std::vector<const Node *> &nodes_list = routes.at(route_index).getNodes();

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
                reverseFarmsOrderInRoute(route_index, pos_left, pos_right);

                long double new_quality = evaluateSolution();
                if(new_quality > old_quality){
                    Utils::debugPrint("2opt: improve quality\n");
                    return true;
                }

                // En caso de que la solución no sea de mejor calidad, se deshace el 2-opt
                // y se prueba con un rango distinto.
                reverseFarmsOrderInRoute(route_index, pos_left, pos_right);
            }
        }
    }

    // Ya se probaron todas las vecindades posibles y ninguna dio una solución de mejor calidad.
    return false;
}

bool Solution::movement_removeOneNode(){
    long double old_quality = evaluateSolution();
    auto quotas_diff(getQuotasDiff());
    bool was_feasible = isFeasible();

    // Randomizar el orden en que se seleccionarán las rutas (route);
    // sin tener que randomizar el vector original.
    std::vector<long> order_of_selected_routes(Utils::range(routes.size()));
    Utils::randomizeVector(order_of_selected_routes);
    for(const long &route_index: order_of_selected_routes){
        const Route &route = routes.at(route_index);
        long old_capacity_left = route.getCapacityLeft();
        const std::vector<const Node *> &nodes_list = route.getNodes();

        // Randomizar el orden en que se borrarán los nodos sin tener que randomizar el vector original.
        std::vector<long> order_of_selected_nodes(Utils::range(nodes_list.size()));
        Utils::randomizeVector(order_of_selected_nodes);
        for(const long &pos: order_of_selected_nodes){
            bool can_be_removed_without_problems = canRemoveFarmFromRoute(route_index, pos);
            if(!can_be_removed_without_problems){
                continue;
            }

            const Node *node = nodes_list.at(pos);
            removeFarmFromRoute(route_index, pos);

            if(!was_feasible){
                bool capacities_improved = didCapacitiesLeftImproved(old_capacity_left, route.getCapacityLeft());
                bool quotas_improved = didQuotasDiffImproved(quotas_diff);
                if(capacities_improved || quotas_improved){
                    Utils::debugPrint("Remove: improve feasibility. ");
                    Utils::debugPrint("\tNode: %ld \tTruck: %ld\n", (*node).getId(), route.getTruckId());
                    return true;
                }
            }
            long double new_quality = evaluateSolution();
            if(new_quality > old_quality){
                Utils::debugPrint("Remove: improve quality\n");
                return true;
            }

            addFarmToRoute(route_index, pos, node);
        }
    }

    // Ya se probaron todas las vecindades posibles y ninguna dio una solución de mejor calidad.
    return false;
}

bool Solution::movement_interchangeNodesBetweenRoutes(){
    long double old_quality = evaluateSolution();
    auto quotas_diff(getQuotasDiff());
    bool was_feasible = isFeasible();

    // Randomizar el orden en que se seleccionarán las rutas de origen (src_route),
    // sin tener que randomizar el vector original.
    std::vector<long> order_of_selected_routes_src(Utils::range(routes.size()));
    Utils::randomizeVector(order_of_selected_routes_src);
    for(const long &src_route_index: order_of_selected_routes_src){
        const Route &src_route = routes.at(src_route_index);
        long src_old_capacity_left = src_route.getCapacityLeft();
        const std::vector<const Node *> &nodes_list_src = src_route.getNodes();

        // Randomizar el orden en que se seleccionarán los nodos de la ruta de origen (node_src),
        // sin tener que randomizar el vector original.
        std::vector<long> order_of_selected_nodes_src(Utils::range(nodes_list_src.size()));
        Utils::randomizeVector(order_of_selected_nodes_src);
        for(const long &src_nodes_index: order_of_selected_nodes_src){
            const Node *node_src = nodes_list_src.at(src_nodes_index);

            // Randomizar el orden en que se seleccionarán las rutas de destino (dst_route),
            // sin tener que randomizar el vector original.
            std::vector<long> order_of_selected_routes_dst(Utils::range(src_route_index + 1, routes.size()));
            Utils::randomizeVector(order_of_selected_routes_dst);
            for(const long &dst_route_index: order_of_selected_routes_dst){
                const Route &dst_route = routes.at(dst_route_index);
                long dst_old_capacity_left = dst_route.getCapacityLeft();
                const std::vector<const Node *> &nodes_list_dst = dst_route.getNodes();

                // Randomizar el orden en que se insertarán los nodos en la ruta de destino,
                // sin tener que randomizar el vector original.
                std::vector<long> order_of_selected_nodes_dst(Utils::range(nodes_list_dst.size()));
                Utils::randomizeVector(order_of_selected_nodes_dst);
                for(const long &dst_nodes_index: order_of_selected_nodes_dst){
                    const Node *node_dst = nodes_list_dst.at(dst_nodes_index);

                    bool interchange_compatible = src_route.isFarmMilkCompatibleWithCurrentMilkType(node_dst) && dst_route.isFarmMilkCompatibleWithCurrentMilkType(node_src);
                    if(!interchange_compatible){
                        continue;
                    }

                    setFarmInRoute(src_route_index, src_nodes_index, node_dst);
                    setFarmInRoute(dst_route_index, dst_nodes_index, node_src);

                    if(!was_feasible){
                        bool capacities_improved = didCapacitiesLeftImproved(src_old_capacity_left, src_route.getCapacityLeft(), dst_old_capacity_left, dst_route.getCapacityLeft());
                        bool quotas_improved = didQuotasDiffImproved(quotas_diff);
                        if(capacities_improved || quotas_improved){
                            Utils::debugPrint("Interchange: improve feasibility. ");
                            Utils::debugPrint("\tNode: %ld \tTruck src: %ld \tTruckdst: %ld\n", (*node_src).getId(), src_route.getTruckId(), dst_route.getTruckId());
                            return true;
                        }
                    }
                    // Si la calidad es mejor, actualizamos la solución y retornamos (Alguna mejora).
                    long double new_quality = evaluateSolution();
                    if(new_quality > old_quality){
                        Utils::debugPrint("Interchange: improve quality\n");
                        Utils::debugPrint("\tNode src: %ld\n\tNode dst: %ld\n\tTruck src: %ld\n\tTruckdst: %ld\n", (*node_src).getId(), (*node_dst).getId(), src_route.getTruckId(), dst_route.getTruckId());
                        return true;
                    }

                    setFarmInRoute(src_route_index, src_nodes_index, node_src);
                    setFarmInRoute(dst_route_index, dst_nodes_index, node_dst);
                }
            }
        }
    }

    // Ya se probaron todas las vecindades posibles y ninguna dio una solución de mejor calidad.
    return false;
}
