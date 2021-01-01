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

    void print(bool newline=false) const;

    [[nodiscard]]
    long getTruckId() const;
    [[nodiscard]]
    char getMilkType() const;
    [[nodiscard]]
    long getCapacityLeft() const;
    const std::vector<const Node *> &getNodes();

    void setTruck(const Truck &truck);
    void addFarm(const Node *farm);
    void addFarm(std::vector<const Node *>::const_iterator &position, const Node *farm);
    void removeFarm(std::vector<const Node *>::const_iterator &position);
    void reverseFarmsOrder(std::vector<const Node *>::size_type left, std::vector<const Node *>::size_type right);

    [[nodiscard]]
    long double evaluateRoute(const Node *initial_node, const std::vector<MilkType> &milk_list) const;

private:
    long truckId;
    long capacityLeft;
    char milkType;
    std::vector<const Node *> nodes; // TODO: considerar cambiar a std::list
    long milkAmount;

    // TODO: contar cuantas granjas hay de cada tipo.
};

#endif
