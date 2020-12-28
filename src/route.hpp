#pragma once
#ifndef ROUTE_HPP
#define ROUTE_HPP

#include <vector>
#include "truck.hpp"
#include "node.hpp"

class Route{
public:
    Route(char milk_type, long nodes_amount);

    void setTruck(Truck &truck);
    void addFarm(Node &farm);

    long getTruckId();
    char getMilkType();
    long getCapacityLeft();

    void print(bool newline=false);

private:
    long truckId;
    long capacityLeft;
    char milkType;
    std::vector<long> nodes;
};

#endif
