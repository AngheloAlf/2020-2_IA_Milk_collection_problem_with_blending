#pragma once
#ifndef ROUTE_HPP
#define ROUTE_HPP

#include <vector>

#include "truck.hpp"
#include "node.hpp"
#include "milk_types_list.hpp"

class Route{
public:
    Route(const MilkTypesList *milk_list, char milk_type, unsigned long nodes_amount, unsigned long milk_types_amount);

    void print(bool newline=false) const;

    [[nodiscard]]
    long getTruckId() const;
    [[nodiscard]]
    char getMilkType() const;
    [[nodiscard]]
    long getMilkAmount() const;
    [[nodiscard]]
    long getCapacityLeft() const;
    [[nodiscard]]
    const std::vector<const Node *> &getNodes() const;

    void setTruck(const Truck &truck);
    void addFarm(const Node *farm);
    void addFarm(long position, const Node *farm);
    void removeFarm(long position);
    void reverseFarmsOrder(long left, long right);

    [[nodiscard]]
    long double evaluateRoute(const Node *initial_node);
    [[nodiscard]]
    long double calculateTransportCosts(const Node *initial_node) const;
    [[nodiscard]]
    long double calculateMilkProfits() const;

private:
    std::vector<const Node *> nodes;
    std::vector<unsigned long> nodes_counter;
    const MilkTypesList *milkList;
    long double quality = 0;
    long truckId;
    long capacityLeft;
    long milkAmount;
    char milkType;
    bool changed = true;
};

#endif
