#include "node.hpp"


Node::Node(long id, long x, long y, char typeProduced, long amountProduced){
    this->id = id;
    this->x = x;
    this->y = y;
    this->typeProduced = typeProduced;
    this->amountProduced = amountProduced;
}
