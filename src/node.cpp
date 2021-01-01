#include "node.hpp"

#include <cstdio>
#include "utils.hpp"

Node::Node(long node_id, long node_x, long node_y, char type_produced, long amount_produced)
: id(node_id), x(node_x), y(node_y), amountProduced(amount_produced), distanceMatrix(nullptr), typeProduced(type_produced){
}

void Node::print(bool newline) const{
    printf("<Node. id: %2ld, x: %2ld, y: %2ld, typeProduced: %c, amountProduced: %4ld>%s", id, x, y, typeProduced, amountProduced, newline?"\n":"");
}

long Node::getId() const{
    return id;
}
char Node::getQuality() const{
    return typeProduced;
}
long Node::getProduced() const{
    return amountProduced;
}

void Node::setDistanceMatrix(long double **&distance_matrix){
    distanceMatrix = distance_matrix;
}

long double Node::distanceTo(const Node &other) const{
    if(distanceMatrix != nullptr){
        return distanceMatrix[id-1][other.id-1];
    }
    return Utils::distance(x, y, other.x, other.y);
}
