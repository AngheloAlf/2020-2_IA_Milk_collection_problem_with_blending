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

void Route::print(bool newline) const{
    printf("<Route. truckId: %li, capacityLeft: %4li, milkType: %c, nodes: [", truckId, capacityLeft, milkType);
    if(nodes.size() > 0){
        printf("%2li", nodes.at(0)->getId());
        for(unsigned long i = 1; i < nodes.size(); ++i){
            printf(", %2li", nodes.at(i)->getId());
        }
    }
    printf("]>%s", newline? "\n" : "");
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
std::vector<const Node *> &Route::getNodes(){
    return nodes;
}

void Route::setTruck(const Truck &truck){
    this->truckId = truck.getId();
    this->capacityLeft = truck.getCapacity();
}
void Route::addFarm(const Node *farm){
    nodes.push_back(farm);
    capacityLeft -= (*farm).getProduced();
}

double Route::evaluateRoute(const Node *initial_node, const std::vector<MilkType> &milk_list) const{
    double result = 0;
    char current_milk_type = this->milkType;
    //printf("Iniciando busqueda...\n");
    auto milk_iter = std::find_if(milk_list.begin(), milk_list.end(), [&current_milk_type](const MilkType &milk){ /*printf("%c %c\n", current_milk_type, milk.id());*/ ; return current_milk_type == milk.getId(); });
    //printf("Busqueda finalizada...\n");
    assert(milk_iter != milk_list.end());

    const Node *prev_node = initial_node;

    double profit_percentage = milk_iter->getMilkProfit();
    for(auto iter = nodes.begin(); iter != nodes.end(); ++iter){
        result += (**iter).getProduced() * profit_percentage;
        result -= (**iter).distanceTo(*prev_node);

        prev_node = *iter;
    }

    result -= (*prev_node).distanceTo(*initial_node);
    return result;
}
