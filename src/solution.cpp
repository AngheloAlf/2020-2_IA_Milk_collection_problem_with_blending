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


bool Solution::addFarmToRoute(long route_index, long position, const Node *farm){
    return routes.at(route_index).addFarm(position, farm);
}
bool Solution::removeFarmFromRoute(long route_index, long position){
    return routes.at(route_index).removeFarm(position);
}
void Solution::reverseFarmsOrderInRoute(long route_index, long left, long right){
    routes.at(route_index).reverseFarmsOrder(left, right);
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

bool Solution::didCapacitiesLeftImproved(long src_route_index, const Route &src_route, const Route &dst_route) const{
    if(dst_route.getCapacityLeft() < 0){
        return false;
    }
    if(routes.at(src_route_index).getCapacityLeft() < 0){
        if(src_route.getCapacityLeft() > routes.at(src_route_index).getCapacityLeft()){
            return true;
        }
    }

    return false;
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
    Solution best_solution(*this);
    Solution solution(*this);

    if(Utils::debugPrintingEnabled) Utils::debugPrint("initial_quality: %Lf\n\n", best_solution.evaluateSolution());

    std::vector<long> order_of_movements(Utils::range(3));

    long i = 0;
    for(; i < K; ++i){
        bool is_better_solution = false;
        Utils::randomizeVector(order_of_movements);
        for(auto &movement: order_of_movements){
            switch (movement){
            case 0:
                is_better_solution |= solution.movement_extraLocalSearch();
                break;
            case 1:
                is_better_solution |= solution.movement_intraLocalSearch();
                break;
            case 2:
                is_better_solution |= solution.movement_removeOneNode();
                break;
            }
        }

        if(Utils::debugPrintingEnabled) Utils::debugPrint("i: %5li - quality: %Lf\n\n", i, solution.evaluateSolution());
        if(is_better_solution){
            best_solution = solution;
        }
        else{
            break;
        }
    }
    Utils::debugPrint("\n");

    /*for(; i < K; ++i){
        bool is_better_solution = solution.movement_removeOneNode();
        if(Utils::debugPrintingEnabled) Utils::debugPrint("i: %5li - quality: %Lf\n\n", i, solution.evaluateSolution());
        if(is_better_solution){
            best_solution = solution;
        }
        else{
            break;
        }
    }*/

    return best_solution;
}


// Mover un nodo de una ruta a las demás rutas.
bool Solution::movement_extraLocalSearch(){
    long double old_quality = evaluateSolution();
    Solution alternative(*this);

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

            bool did_quality_improved = alternative.tryMoveNodeBetweenRoutes(*this, old_quality, src_route_index, dst_route_index);
            if(did_quality_improved){
                //solution = alternative;
                *this = alternative;
                return true;
            }
        }
    }

    // Ya se probaron todas las vecindades posibles y ninguna dio una solución de mejor calidad.
    return false;
}

// Movimiento 2-opt de la ruta consigo misma.
bool Solution::movement_intraLocalSearch(){
    long double old_quality = evaluateSolution();
    Solution alternative(*this);

    const std::vector<Route> &routes_list = alternative.getRoutes();

    // Randomizar el orden en que se seleccionarán las rutas (route);
    // sin tener que randomizar el vector original.
    std::vector<long> order_of_selected_routes(Utils::range(routes_list.size()));
    Utils::randomizeVector(order_of_selected_routes);

    for(const long &route_index: order_of_selected_routes){
        //Route &route = alternative.at(route_index);

        bool did_quality_improved = alternative.try2OptInRoute(old_quality, route_index);
        if(did_quality_improved){
            //solution = alternative;
            *this = alternative;
            return true;
        }
    }

    // Ya se probaron todas las vecindades posibles y ninguna dio una solución de mejor calidad.
    return false;
}

bool Solution::movement_removeOneNode(){
    long double old_quality = evaluateSolution();
    Solution alternative(*this);

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
                    //solution = alternative;
                    *this = alternative;
                    return true;
                }
            }
            alternative.addFarmToRoute(route_index, pos, node);
        }
    }

    // Ya se probaron todas las vecindades posibles y ninguna dio una solución de mejor calidad.
    return false;
}


bool Solution::tryMoveNodeBetweenRoutes(const Solution &original_solution, long double old_quality, long src_route_index, long dst_route_index){
    auto quotas_diff(getQuotasDiff());
    bool was_feasible = isFeasible();

    const Route &src_route = routes.at(src_route_index);
    const Route &dst_route = routes.at(dst_route_index);

    const std::vector<const Node *> &nodes_list_src = routes.at(src_route_index).getNodes();

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
        bool removed_without_problems = removeFarmFromRoute(src_route_index, src_nodes_index);
        if(removed_without_problems){
            const std::vector<const Node *> &nodes_list_dst = dst_route.getNodes();

            // Randomizar el orden en que se insertarán los nodos en la ruta de destino,
            // sin tener que randomizar el vector original.
            std::vector<long> order_of_selected_nodes_dst(Utils::range(nodes_list_dst.size()));
            Utils::randomizeVector(order_of_selected_nodes_dst);

            for(const long &dst_nodes_index: order_of_selected_nodes_dst){
                // Se agrega el nodo a la ruta de destino en la posición correspondiente
                // y calcular la calidad de esta nueva solución.
                bool added_without_problems = addFarmToRoute(dst_route_index, dst_nodes_index, node_src);

                if(added_without_problems){
                    if(!was_feasible){
                        bool capacities_improved = original_solution.didCapacitiesLeftImproved(src_route_index, src_route, dst_route);
                        bool quotas_improved = didQuotasDiffImproved(quotas_diff);
                        if(capacities_improved || quotas_improved){
                            Utils::debugPrint("Move: improve feasibility\n");
                            Utils::debugPrint("\tNode: %ld\n\tTruck src: %ld\n\tTruckdst: %ld\n", (*node_src).getId(), src_route.getTruckId(), dst_route.getTruckId());
                            return true;
                        }
                    }
                    else{
                        // Si la calidad es mejor, actualizamos la solución y retornamos (Alguna mejora).
                        long double new_quality = evaluateSolution();
                        if(new_quality > old_quality){
                            Utils::debugPrint("Move: improve quality\n");
                            Utils::debugPrint("\tNode: %ld\n\tTruck src: %ld\n\tTruckdst: %ld\n", (*node_src).getId(), src_route.getTruckId(), dst_route.getTruckId());
                            return true;
                        }
                    }
                }
                // Si la nueva calidad no supera la calidad anterior, quitamos el nodo de
                // esa posición y reintentamos en la siguiente posición.
                removeFarmFromRoute(dst_route_index, dst_nodes_index);
            }
        }

        // Insertar este nodo en esta ruta destino no dio mejoras, por lo que recolocamos
        // el nodo en su ruta original, y en su posición original.
        addFarmToRoute(src_route_index, src_nodes_index, node_src);
    }

    return false;
}

bool Solution::try2OptInRoute(long double old_quality, long route_index){
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

    return false;
}

