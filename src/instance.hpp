#pragma once
#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include <stdio.h>
#include <vector>

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

    std::vector<Route> initialSolution() const;
    double evaluateSolution(std::vector<Route> &sol) const;

    std::vector<Route> hillClimbing(std::vector<Route> &initial_solution, long K) const;

private:
    long trucksAmount;
    std::vector<Truck> trucksList;

    long milkTypesAmount;
    std::vector<MilkType> milkList;

    long nodesAmount;
    std::vector<Node> nodesList;

    double **distanceBetweenNodes;

    // Movimientos para hill climbing.
    bool extraLocalSearch(std::vector<Route> &solution) const;
    bool intraLocalSearch(std::vector<Route> &solution) const;

    // Funciones de inicialización.
    void readTrucks(FILE *arch);
    void readMilk(FILE *arch);
    void readNodes(FILE *arch);
};

#endif
