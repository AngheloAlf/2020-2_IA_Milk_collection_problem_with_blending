#pragma once
#ifndef NODE_HPP
#define NODE_HPP

/**
 *  Modela cada planta procesadora
 */
class Node{
public:
    Node(long node_id, long node_x, long node_y, char type_produced, long amount_produced);

    void print(bool newline=false) const;

    [[nodiscard]]
    long getId() const;
    [[nodiscard]]
    char getQuality() const;
    [[nodiscard]]
    long getProduced() const;

    void setDistanceMatrix(long double **&distance_matrix);

    [[nodiscard]]
    long double distanceTo(const Node &other) const;
    [[nodiscard]]
    long double cachedDistance(const Node &other) const;

private:
    long id;
    long x;
    long y;
    long amountProduced; // Cantidad de leche producida.
    long double **distanceMatrix;
    char typeProduced; // Tipo de leche producida. 'A', 'B', 'C', etc.
};

#endif
