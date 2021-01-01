#pragma once
#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include <cstdio>
#include <vector>
#include <memory>

#include "truck.hpp"
#include "milk_type.hpp"
#include "node.hpp"
#include "route.hpp"

/**
 *  Clase que modela la instancia que proviene de un archivo de texto.
 */
class Instance{
public:
    Instance(char *filename);
    ~Instance();

    void print(bool newline=false) const;

    [[nodiscard]]
    std::vector<Route> initialSolution() const;
    [[nodiscard]]
    long double evaluateSolution(const std::vector<Route> &sol) const;

    [[nodiscard]]
    std::vector<Route> hillClimbing(const std::vector<Route> &initial_solution, long K) const;

private:
    unsigned long trucksAmount;
    std::vector<Truck> trucksList;

    unsigned long milkTypesAmount;
    std::vector<MilkType> milkList;

    unsigned long nodesAmount;
    std::vector<std::unique_ptr<Node>> nodesList;

    long double **distanceBetweenNodes;

    // Movimientos para hill climbing.
    // Mover un nodo de una ruta a las demás rutas.
    bool extraLocalSearch(std::vector<Route> &solution) const;
    // Movimiento 2-opt de la ruta consigo misma.
    bool intraLocalSearch(std::vector<Route> &solution) const;

    // Funciones de inicialización.
    void readTrucks(FILE *arch);
    void readMilk(FILE *arch);
    void readNodes(FILE *arch);
};

#endif
