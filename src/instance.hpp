#pragma once
#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include <cstdio>
#include <vector>
#include <memory>

#include "truck.hpp"
#include "milk_types_list.hpp"
#include "node.hpp"
#include "route.hpp"
#include "solution.hpp"

/**
 *  Clase que modela la instancia que proviene de un archivo de texto.
 */
class Instance{
public:
    Instance(char *filename);
    Instance(Instance &other) = delete;
    Instance(Instance &&other) = delete;
    ~Instance();

    Instance &operator=(Instance &other) = delete;
    Instance &&operator=(Instance &&other) = delete;

    void print(bool newline=false) const;

    [[nodiscard]]
    Node *getInitialNode() const;

    [[nodiscard]]
    Solution initialSolution();

    [[nodiscard]]
    std::vector<long> getQuotasDiff(const std::vector<Route> &sol) const;
    [[nodiscard]]
    bool didQuotasDiffImproved(const std::vector<long> &quotas_diff, const Solution &sol) const;
    [[nodiscard]]
    bool didCapacitiesLeftImproved(const Solution &original_solution, long src_route_index, const Route &src_route, const Route &dst_route) const;

    [[nodiscard]]
    Solution hillClimbing(const Solution &initial_solution, long K) const;

private:
    unsigned long trucksAmount;
    std::vector<Truck> trucksList;

    MilkTypesList milkList;

    unsigned long nodesAmount;
    std::vector<std::unique_ptr<Node>> nodesList;

    long double **distanceBetweenNodes;

    // Movimientos para hill climbing.
    // Mover un nodo de una ruta a las demás rutas.
    [[nodiscard]]
    bool movement_extraLocalSearch(Solution &solution) const;
    // Movimiento 2-opt de la ruta consigo misma.
    [[nodiscard]]
    bool movement_intraLocalSearch(Solution &solution) const;
    [[nodiscard]]
    bool movement_removeOneNode(Solution &solution) const;

    bool tryMoveNodeBetweenRoutes(const Solution &original_solution, Solution &alternative, long double old_quality, long src_route_index, long dst_route_index) const;
    bool try2OptInRoute(Solution &alternative, long double old_quality, long route_index) const;

    // Funciones de inicialización.
    void readTrucks(FILE *arch);
    void readMilk(FILE *arch);
    void readNodes(FILE *arch);
};

#endif
