#include "solution.hpp"

#include <cassert>

#include "utils.hpp"


Solution Solution::initialSolution(const std::vector<std::unique_ptr<Node>> &nodes_list, MilkTypesList *milk_list, const std::vector<Truck> &trucks_list, Node *initial_node){
    Solution sol;
    sol.initialNode = initial_node;
    sol.milkList = milk_list;
    sol.routes.reserve(trucks_list.size());
    //std::vector<Route> routes;
    //routes.reserve(trucks_list.size());
    char milk_type = 'A';
    for(unsigned long i = 0; i < trucks_list.size(); ++i){
        sol.routes.emplace_back(milk_list, milk_type++, nodes_list.size(), (*milk_list).getTotal());
    }

    bool valid = false;
    while(!valid){
        valid = true;
        Utils::randomizeVector(sol.routes);

        for(unsigned long i = 0; valid && i < trucks_list.size(); ++i){
            const auto &truck = trucks_list.at(i);
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
    farms_list.reserve(nodes_list.size());
    for(auto iter = nodes_list.begin()+1; iter < nodes_list.end(); ++iter){
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

        for(unsigned long i = 0; i < trucks_list.size(); ++i){
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
    quotas_aux.reserve((*milkList).getTotal());
    quotas_aux.clear();

    for(const auto &milk: *milkList){
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


long double Solution::evaluateSolution(){
    long double result = 0;

    for(Route &route: routes){
        result += route.evaluateRoute(initialNode);
    }

    return result;
}
long double Solution::calculateTransportCosts() const{
    long double result = 0;

    for(const Route &route: routes){
        result += route.calculateTransportCosts(initialNode);
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


