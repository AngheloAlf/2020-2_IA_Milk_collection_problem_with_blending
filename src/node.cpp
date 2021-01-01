#include "node.hpp"

#include <cstdio>
#include "utils.hpp"

Node::Node(long _id, long x, long y, char typeProduced, long amountProduced){
    this->_id = _id;
    this->x = x;
    this->y = y;
    this->typeProduced = typeProduced;
    this->amountProduced = amountProduced;
}

void Node::print(bool newline) const{
    printf("<Node. id: %2ld, x: %2ld, y: %2ld, typeProduced: %c, amountProduced: %4ld>%s", _id, x, y, typeProduced, amountProduced, newline?"\n":"");
}

long Node::getId() const{
    return _id;
}
char Node::getQuality() const{
    return typeProduced;
}
long Node::getProduced() const{
    return amountProduced;
}

long double Node::distanceTo(const Node &other) const{
    return Utils::distance(x, y, other.x, other.y);
}
