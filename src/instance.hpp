#pragma once
#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include <stdio.h>
#include <vector>

#include "truck.hpp"
#include "milk_type.hpp"
#include "node.hpp"

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

public:
    Instance(char *filename);
    ~Instance();

    std::vector<std::vector<long>> initialSolution();
    void print();
};



#endif
