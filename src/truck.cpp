#include "truck.hpp"

#include <cstdio>


Truck::Truck(long truck_id, long truck_capacity)
: id(truck_id), capacity(truck_capacity){
}

void Truck::print(bool newline) const{
    printf("<Truck. id: %ld, capacity: %ld>%s", this->id, this->capacity, newline?"\n":"");
}

long Truck::getId() const{
    return id;
}
long Truck::getCapacity() const{
    return capacity;
}
