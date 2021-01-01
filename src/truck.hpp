#pragma once
#ifndef TRUCK_HPP
#define TRUCK_HPP

/**
 *  Clase que modela a un camión.
 */
class Truck{
private:
    long _id;
    long _capacity; // Capacidad total de este camión.

public:
    Truck(long _id, long _capacity);

    void print(bool newline=false) const;

    [[nodiscard]]
    long getId() const;
    [[nodiscard]]
    long getCapacity() const;
};



#endif
