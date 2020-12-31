#pragma once
#ifndef NODE_HPP
#define NODE_HPP

/**
 *  Modela cada planta procesadora
 */
class Node{
private:
    long _id;
    long x;
    long y;
    char typeProduced; // Tipo de leche producida. 'A', 'B', 'C', etc.
    long amountProduced; // Cantidad de leche producida.

public:
    Node(long _id, long x, long y, char typeProduced, long amountProduced);

    void print(bool newline=false) const;

    double distanceTo(const Node &other) const;
    long id() const;
    char quality() const;
    long produced() const;
};



#endif
