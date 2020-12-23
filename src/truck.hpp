#pragma once
#ifndef TRUCK_HPP
#define TRUCK_HPP

/**
 *  Clase que modela a un camión.
 */
class Truck{
private:
    long capacity; // Capacidad total de este camión.

public:
    Truck(long capacity);

    void print();
};



#endif
