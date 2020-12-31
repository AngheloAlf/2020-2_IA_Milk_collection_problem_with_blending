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

    void setTruck(const Truck &truck);
    void addFarm(const Node &farm);

    long getTruckId() const;
    char getMilkType() const;
    long getCapacityLeft() const;
    std::vector<long> &getNodes();

    double evaluateRoute(const std::vector<Node> &farms_list, const std::vector<MilkType> &milk_list) const;

    void print(bool newline=false) const;

private:
    long truckId;
    long capacityLeft;
    char milkType;
    std::vector<long> nodes;
};

#endif
