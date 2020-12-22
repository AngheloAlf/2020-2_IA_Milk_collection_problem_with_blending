#include "node.hpp"

#include <stdio.h>
#include "utils.hpp"

Node::Node(long id, long x, long y, char typeProduced, long amountProduced){
    this->id = id;
    this->x = x;
    this->y = y;
    this->typeProduced = typeProduced;
    this->amountProduced = amountProduced;
}

void Node::print(){
    printf("<Node. id: %2ld, x: %2ld, y: %2ld, typeProduced: %c, amountProduced: %4ld>", id, x, y, typeProduced, amountProduced);
}

double Node::distanceTo(Node &other){
    return distance(x, y, other.x, other.y);
}
