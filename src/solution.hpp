#pragma once
#ifndef SOLUTION_HPP
#define SOLUTION_HPP

#include <memory>
#include <vector>

#include "route.hpp"
#include "node.hpp"
#include "milk_types_list.hpp"
#include "truck.hpp"

class Solution{
public:
    static Solution initialSolution(const std::vector<std::unique_ptr<Node>> &nodes_list, MilkTypesList *milk_list, const std::vector<Truck> &trucks_list, Node *initial_node);

    void print(bool newline=false) const;

    [[nodiscard]]
    const std::vector<Route> &getRoutes() const;

    [[nodiscard]]
    bool isFeasible() const;

    bool addFarmToRoute(long route_index, long position, const Node *farm);
    bool removeFarmFromRoute(long route_index, long position);
    void reverseFarmsOrderInRoute(long route_index, long left, long right);

    [[nodiscard]]
    long double evaluateSolution();
    [[nodiscard]]
    long double calculateTransportCosts() const;
    [[nodiscard]]
    long double calculateMilkProfits() const;

private:
    std::vector<Route> routes;
    Node *initialNode;
    MilkTypesList *milkList;
};

#endif
