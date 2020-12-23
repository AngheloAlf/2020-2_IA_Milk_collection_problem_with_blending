#pragma once
#ifndef NODE_HPP
#define NODE_HPP

/**
 *  Modela cada planta procesadora
 */
class Node{
private:
    long id;
    long x;
    long y;
    char typeProduced; // Tipo de leche producida. 'A', 'B', 'C', etc.
    long amountProduced; // Cantidad de leche producida.

public:
    Node(long id, long x, long y, char typeProduced, long amountProduced);

    void print();

    double distanceTo(Node &other);
};



#endif
