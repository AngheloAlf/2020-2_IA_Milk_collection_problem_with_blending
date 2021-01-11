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
    Instance(const char *filename);
    Instance(Instance &other) = delete;
    Instance(Instance &&other) = delete;
    ~Instance();

    Instance &operator=(Instance &other) = delete;
    Instance &&operator=(Instance &&other) = delete;

    void print(bool newline=false) const;

    [[nodiscard]]
    Node *getInitialNode() const;
    [[nodiscard]]
    const std::vector<Truck> &getTrucksList() const;
    [[nodiscard]]
    const MilkTypesList &getMilkList() const;
    [[nodiscard]]
    const std::vector<std::unique_ptr<Node>> &getNodesList() const;

private:
    unsigned long trucksAmount;
    std::vector<Truck> trucksList;

    MilkTypesList milkList;

    unsigned long nodesAmount;
    std::vector<std::unique_ptr<Node>> nodesList;

    long double **distanceBetweenNodes;

    // Funciones de inicialización.
    void readTrucks(FILE *arch);
    void readMilk(FILE *arch);
    void readNodes(FILE *arch);
};

#endif
