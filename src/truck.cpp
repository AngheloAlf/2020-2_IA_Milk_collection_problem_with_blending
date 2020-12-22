#include "truck.hpp"

#include <stdio.h>


Truck::Truck(long capacity){
    this->capacity = capacity;
}

void Truck::print(){
    printf("<Truck. capacity: %ld>", this->capacity);
}

