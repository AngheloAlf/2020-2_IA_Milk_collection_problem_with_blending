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
private:
    long trucksAmount;
    std::vector<Truck> trucksList;

    long milkTypesAmount;
    std::vector<MilkType> milkList;

    long nodesAmount;
    std::vector<Node> nodesList;

    double **distanceBetweenNodes;

    void readTrucks(FILE *arch);
    void readMilk(FILE *arch);
    void readNodes(FILE *arch);

    bool extraLocalSearch(std::vector<Route> &solution);
    bool intraLocalSearch(std::vector<Route> &solution);

public:
    Instance(char *filename);
    ~Instance();

    std::vector<Route> initialSolution();
    double evaluateSolution(std::vector<Route> &sol);

    std::vector<Route> hillClimbing(std::vector<Route> &initial_solution, long K);

    void print();
};

#endif
