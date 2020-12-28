#include "truck.hpp"

#include <stdio.h>


Truck::Truck(long _capacity){
    this->_capacity = _capacity;
}

void Truck::print(){
    printf("<Truck. capacity: %ld>", this->_capacity);
}

long Truck::capacity(){
    return _capacity;
}
