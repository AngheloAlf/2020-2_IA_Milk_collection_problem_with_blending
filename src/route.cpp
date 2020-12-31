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

void Route::setTruck(const Truck &truck){
    this->truckId = truck.id();
    this->capacityLeft = truck.capacity();
}
void Route::addFarm(const Node &farm){
    nodes.push_back(farm.id());
    capacityLeft -= farm.produced();
}

long Route::getTruckId() const{
    return truckId;
}
char Route::getMilkType() const{
    return milkType;
}
long Route::getCapacityLeft() const{
    return capacityLeft;
}
std::vector<long> &Route::getNodes(){
    return nodes;
}

double Route::evaluateRoute(const std::vector<Node> &farms_list, const std::vector<MilkType> &milk_list) const{
    double result = 0;
    char current_milk_type = this->milkType;
    //printf("Iniciando busqueda...\n");
    auto milk_iter = std::find_if(milk_list.begin(), milk_list.end(), [&current_milk_type](const MilkType &milk){ /*printf("%c %c\n", current_milk_type, milk.id());*/ ; return current_milk_type == milk.id(); });
    //printf("Busqueda finalizada...\n");
    assert(milk_iter != milk_list.end());

    const Node &initial_node = farms_list.at(0);
    //Node &prev_node = farms_list.at(0);
    std::vector<Node>::const_iterator prev_iter = farms_list.begin();

    double profit_percentage = milk_iter->milkProfit();
    for(const long &node_id: nodes){
        auto iter = std::find_if(farms_list.begin(), farms_list.end(), [&node_id](const Node &node){ return node.id() == node_id; });
        assert(iter != farms_list.end());
        const Node &curr_node = *iter;

        result += curr_node.produced() * profit_percentage;
        //result -= curr_node.distanceTo(prev_node);
        result -= curr_node.distanceTo(*prev_iter);

        //prev_node = curr_node;
        prev_iter = iter;
    }

    //result -= prev_node.distanceTo(initial_node);
    result -= (*prev_iter).distanceTo(initial_node);
    return result;
}

void Route::print(bool newline) const{
    printf("<Route. truckId: %li, capacityLeft: %4li, milkType: %c, nodes: [", truckId, capacityLeft, milkType);
    if(nodes.size() > 0){
        printf("%2li", nodes.at(0));
        for(unsigned long i = 1; i < nodes.size(); ++i){
            printf(", %2li", nodes.at(i));
        }
    }
    printf("]>%s", newline? "\n" : "");
}
