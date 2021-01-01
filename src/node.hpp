#pragma once
#ifndef NODE_HPP
#define NODE_HPP

/**
 *  Modela cada planta procesadora
 */
class Node{
public:
    Node(long _id, long x, long y, char typeProduced, long amountProduced);

    void print(bool newline=false) const;

    [[nodiscard]]
    long getId() const;
    [[nodiscard]]
    char getQuality() const;
    [[nodiscard]]
    long getProduced() const;

    [[nodiscard]]
    long double distanceTo(const Node &other) const;

private:
    long _id;
    long x;
    long y;
    char typeProduced; // Tipo de leche producida. 'A', 'B', 'C', etc.
    long amountProduced; // Cantidad de leche producida.
};

#endif
