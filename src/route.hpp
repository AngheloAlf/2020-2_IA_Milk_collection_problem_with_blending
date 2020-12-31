#pragma once
#ifndef ROUTE_HPP
#define ROUTE_HPP

#include <vector>
#include "truck.hpp"
#include "node.hpp"
#include "milk_type.hpp"

class Route{
public:
    Route(char milk_type, long nodes_amount);
    Route(const Route &other);

    Route &operator=(const Route &other);

    void print(bool newline=false) const;

    long getTruckId() const;
    char getMilkType() const;
    long getCapacityLeft() const;
    std::vector<long> &getNodes();

    void setTruck(const Truck &truck);
    void addFarm(const Node &farm);

    double evaluateRoute(const std::vector<Node> &farms_list, const std::vector<MilkType> &milk_list) const;

private:
    long truckId;
    long capacityLeft;
    char milkType;
    std::vector<long> nodes;
};

#endif
