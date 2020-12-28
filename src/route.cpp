#include "route.hpp"

#include <stdio.h>
#include <assert.h>
#include <algorithm>


Route::Route(char milk_type, long nodes_amount)
: milkType(milk_type){
    nodes.reserve(nodes_amount);
}

void Route::setTruck(Truck &truck){
    this->truckId = truck.id();
    this->capacityLeft = truck.capacity();
}
void Route::addFarm(Node &farm){
    nodes.push_back(farm.id());
    capacityLeft -= farm.produced();
}

long Route::getTruckId(){
    return truckId;
}
char Route::getMilkType(){
    return milkType;
}
long Route::getCapacityLeft(){
    return capacityLeft;
}

double Route::evaluateRoute(std::vector<Node> &farms_list, std::vector<MilkType> &milk_list){
    double result = 0;
    char current_milk_type = this->milkType;
    auto milk_iter = std::find_if(milk_list.begin(), milk_list.end(), [&current_milk_type](MilkType &milk){ return current_milk_type == milk.id(); });
    assert(milk_iter != milk_list.end());

    double profit_percentage = milk_iter->milkProfit();
    for(long &node_id: nodes){
        auto iter = std::find_if(farms_list.begin(), farms_list.end(), [&node_id](Node &node){ return node.id() == node_id; });
        assert(iter != farms_list.end());
        result += iter->produced() * profit_percentage;
    }
    return result;
}

void Route::print(bool newline){
    printf("<Route. truckId: %li, capacityLeft: %4li, milkType: %c, nodes: [", truckId, capacityLeft, milkType);
    if(nodes.size() > 0){
        printf("%2li", nodes.at(0));
        for(unsigned long i = 1; i < nodes.size(); ++i){
            printf(", %2li", nodes.at(i));
        }
    }
    printf("]>%s", newline? "\n" : "");
}
