#include "route.hpp"

#include "stdio.h"


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

void Route::print(bool newline){
    printf("<Route. truckId: %li, capacityLeft: %4li, milkType: %c, nodes: [", truckId, capacityLeft, milkType);
    if(nodes.size() > 0){
        printf("%li", nodes.at(0));
        for(unsigned long i = 1; i < nodes.size(); ++i){
            printf(", %li", nodes.at(i));
        }
    }
    printf("]>%s", newline? "\n" : "");
}
