#include "route.hpp"

#include <stdio.h>
#include <assert.h>
#include <algorithm>


Route::Route(char milk_type, long nodes_amount)
: milkType(milk_type){
    nodes.reserve(nodes_amount);
}
Route::Route(const Route &other)
: truckId(other.truckId), capacityLeft(other.capacityLeft), milkType(other.milkType), nodes(other.nodes){
}

Route &Route::operator=(const Route &other){
    truckId = other.truckId;
    capacityLeft = other.capacityLeft;
    milkType = other.milkType;
    nodes = other.nodes;
    return *this;
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
std::vector<long> &Route::getNodes(){
    return nodes;
}

double Route::evaluateRoute(std::vector<Node> &farms_list, std::vector<MilkType> &milk_list){
    double result = 0;
    char current_milk_type = this->milkType;
    //printf("Iniciando busqueda...\n");
    auto milk_iter = std::find_if(milk_list.begin(), milk_list.end(), [&current_milk_type](MilkType &milk){ /*printf("%c %c\n", current_milk_type, milk.id());*/ ; return current_milk_type == milk.id(); });
    //printf("Busqueda finalizada...\n");
    if(milk_iter == milk_list.end()){
        printf("oh no\n");
    }
    assert(milk_iter != milk_list.end());

    Node &initial_node = farms_list.at(0);
    Node &prev_node = farms_list.at(0);

    double profit_percentage = milk_iter->milkProfit();
    for(long &node_id: nodes){
        auto iter = std::find_if(farms_list.begin(), farms_list.end(), [&node_id](Node &node){ return node.id() == node_id; });
        assert(iter != farms_list.end());
        Node &curr_node = *iter;

        result += curr_node.produced() * profit_percentage;
        result -= curr_node.distanceTo(prev_node);

        prev_node = curr_node;
    }

    result -= prev_node.distanceTo(initial_node);
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
