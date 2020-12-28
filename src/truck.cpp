#include "truck.hpp"

#include <stdio.h>


Truck::Truck(long _id, long _capacity){
    this->_id = _id;
    this->_capacity = _capacity;
}

void Truck::print(){
    printf("<Truck. id: %ld, capacity: %ld>", this->_id, this->_capacity);
}

long Truck::id(){
    return _id;
}
long Truck::capacity(){
    return _capacity;
}
