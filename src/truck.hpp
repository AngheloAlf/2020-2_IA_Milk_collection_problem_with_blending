#pragma once
#ifndef TRUCK_HPP
#define TRUCK_HPP

/**
 *  Clase que modela a un camión.
 */
class Truck{
public:
    Truck(long truck_id, long truck_capacity);

    void print(bool newline=false) const;

    [[nodiscard]]
    long getId() const;
    [[nodiscard]]
    long getCapacity() const;

private:
    long id;
    long capacity; // Capacidad total de este camión.
};



#endif
