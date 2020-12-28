#pragma once
#ifndef ROUTE_HPP
#define ROUTE_HPP

#include <vector>

class Route{
public:
    Route(char milk_type, long nodes_amount);

    void setTruck(long truck_id);
    void addFarm(long farm_id);

    void print(bool newline=false);

private:
    long truckId;
    char milkType;
    std::vector<long> nodes;
};

#endif
