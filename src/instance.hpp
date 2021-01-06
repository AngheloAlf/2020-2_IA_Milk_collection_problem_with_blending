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
    const Node *getInitialNode() const;

    [[nodiscard]]
    std::vector<Route> initialSolution() const;
    [[nodiscard]]
    long double evaluateSolution(std::vector<Route> &sol) const;
    [[nodiscard]]
    long double calculateTransportCosts(const std::vector<Route> &sol) const;
    [[nodiscard]]
    long double calculateMilkProfits(const std::vector<Route> &sol) const;

    [[nodiscard]]
    std::vector<Route> hillClimbing(const std::vector<Route> &initial_solution, long K) const;

private:
    unsigned long trucksAmount;
    std::vector<Truck> trucksList;

    MilkTypesList milkList;

    unsigned long nodesAmount;
    std::vector<std::unique_ptr<Node>> nodesList;

    long double **distanceBetweenNodes;

    // Movimientos para hill climbing.
    // Mover un nodo de una ruta a las demás rutas.
    bool extraLocalSearch(std::vector<Route> &solution) const;
    // Movimiento 2-opt de la ruta consigo misma.
    bool intraLocalSearch(std::vector<Route> &solution) const;

    bool tryMoveNodeBetweenRoutes(std::vector<Route> &alternative, long double old_quality, Route &src_route, Route &dst_route) const;
    bool try2OptInRoute(std::vector<Route> &alternative, long double old_quality, Route &route) const;

    // Funciones de inicialización.
    void readTrucks(FILE *arch);
    void readMilk(FILE *arch);
    void readNodes(FILE *arch);
};

#endif
