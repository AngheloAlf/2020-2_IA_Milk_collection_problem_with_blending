#pragma once
#ifndef SOLUTION_HPP
#define SOLUTION_HPP

#include <memory>
#include <vector>

#include "route.hpp"
#include "node.hpp"
#include "milk_types_list.hpp"
#include "truck.hpp"
#include "instance.hpp"

class Solution{
public:
    Solution(const Instance *instance);

    static Solution initialSolution(const Instance *instance);

    void print(bool newline=false) const;

    [[nodiscard]]
    const std::vector<Route> &getRoutes() const;

    [[nodiscard]]
    bool isFeasible() const;

    bool addFarmToRoute(long route_index, long position, const Node *farm);
    bool removeFarmFromRoute(long route_index, long position);
    void reverseFarmsOrderInRoute(long route_index, long left, long right);

    [[nodiscard]]
    std::vector<long> getQuotasDiff() const;
    [[nodiscard]]
    bool didQuotasDiffImproved(const std::vector<long> &quotas_diff) const;
    [[nodiscard]]
    bool didCapacitiesLeftImproved(long src_route_index, const Route &src_route) const;
    [[nodiscard]]
    bool didCapacitiesLeftImproved(long src_route_index, const Route &src_route, const Route &dst_route) const;

    [[nodiscard]]
    long double evaluateSolution();
    [[nodiscard]]
    long double calculateTransportCosts() const;
    [[nodiscard]]
    long double calculateMilkProfits() const;

    [[nodiscard]]
    Solution hillClimbing(long K) const;

private:
    std::vector<Route> routes;
    const Instance *instance;

    // Movimientos para hill climbing.
    // Mover un nodo de una ruta a las demás rutas.
    [[nodiscard]]
    bool movement_extraLocalSearch();
    // Movimiento 2-opt de la ruta consigo misma.
    [[nodiscard]]
    bool movement_intraLocalSearch();
    [[nodiscard]]
    bool movement_removeOneNode();

    [[nodiscard]]
    bool tryMoveNodeBetweenRoutes(const Solution &original_solution, long double old_quality, long src_route_index, long dst_route_index);
    [[nodiscard]]
    bool try2OptInRoute(long double old_quality, long route_index);
};

#endif
