#include "truck.hpp"

#include <cstdio>


Truck::Truck(long _id, long _capacity){
    this->_id = _id;
    this->_capacity = _capacity;
}

void Truck::print(bool newline) const{
    printf("<Truck. id: %ld, capacity: %ld>%s", this->_id, this->_capacity, newline?"\n":"");
}

long Truck::getId() const{
    return _id;
}
long Truck::getCapacity() const{
    return _capacity;
}
