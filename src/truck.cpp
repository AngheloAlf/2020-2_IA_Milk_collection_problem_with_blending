#include "truck.hpp"

#include <stdio.h>


Truck::Truck(long _id, long _capacity){
    this->_id = _id;
    this->_capacity = _capacity;
}

void Truck::print(bool newline) const{
    printf("<Truck. id: %ld, capacity: %ld>%s", this->_id, this->_capacity, newline?"\n":"");
}

long Truck::id() const{
    return _id;
}
long Truck::capacity() const{
    return _capacity;
}
