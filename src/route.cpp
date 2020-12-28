#include "route.hpp"

#include "stdio.h"


Route::Route(char milk_type, long nodes_amount)
: milkType(milk_type){
    nodes.reserve(nodes_amount);
}

void Route::setTruck(long truck_id){
    this->truckId = truck_id;
}
void Route::addFarm(long farm_id){
    nodes.push_back(farm_id);
}

long Route::getTruckId(){
    return truckId;
}
char Route::getMilkType(){
    return milkType;
}

void Route::print(bool newline){
    printf("<Route. truckId: %li, milkType: %c, nodes: [", truckId, milkType);
    if(nodes.size() > 0){
        printf("%li", nodes.at(0));
        for(unsigned long i = 1; i < nodes.size(); ++i){
            printf(", %li", nodes.at(i));
        }
    }
    printf("]>%s", newline? "\n" : "");
}
