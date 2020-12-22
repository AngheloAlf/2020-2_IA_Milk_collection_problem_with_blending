#pragma once
#ifndef NODE_HPP
#define NODE_HPP


class Node{
private:
    long id;
    long x;
    long y;
    char typeProduced;
    long amountProduced;

public:
    Node(long id, long x, long y, char typeProduced, long amountProduced);

    void print();

    double distanceTo(Node &other);
};



#endif
