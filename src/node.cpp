#include "node.hpp"

#include <stdio.h>
#include "utils.hpp"

Node::Node(long _id, long x, long y, char typeProduced, long amountProduced){
    this->_id = _id;
    this->x = x;
    this->y = y;
    this->typeProduced = typeProduced;
    this->amountProduced = amountProduced;
}

void Node::print(){
    printf("<Node. id: %2ld, x: %2ld, y: %2ld, typeProduced: %c, amountProduced: %4ld>", _id, x, y, typeProduced, amountProduced);
}

double Node::distanceTo(Node &other){
    return distance(x, y, other.x, other.y);
}

long Node::id(){
    return _id;
}
char Node::quality(){
    return typeProduced;
}
long Node::produced(){
    return amountProduced;
}
