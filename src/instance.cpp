#include "instance.hpp"

#include <cstdlib>
#include <cassert>
#include <cmath>

#include "utils.hpp"


Instance::Instance(const char *filename){
    FILE *arch = fopen(filename, "r");
    assert(arch != nullptr);
    if(arch == nullptr){
        fprintf(stderr, "No se encontró el archivo '%s'.\n", filename);
        exit(-1);
    }

    readTrucks(arch);
    readMilk(arch);
    readNodes(arch);

    fclose(arch);

    assert(nodesAmount != 0);
    distanceBetweenNodes = (long double **)malloc(sizeof(long double *)*(nodesAmount+1));
    assert(distanceBetweenNodes != nullptr);
    for(unsigned long i = 1; i < nodesAmount+1; ++i){
        distanceBetweenNodes[i] = (long double *)malloc(sizeof(long double)*(nodesAmount+1));
        assert(distanceBetweenNodes[i] != nullptr);
        auto *node_i = nodesList.at(i-1).get();
        // Los nodos tienen distancia cero a si mismos.
        distanceBetweenNodes[i][i] = 0;

        for(unsigned long j = i+1; j < nodesAmount+1; ++j){
            long double dist = (*node_i).distanceTo(*nodesList.at(j-1).get());
            assert(!std::isnan(dist));
            assert(!std::isinf(dist));
            assert(std::isfinite(dist));
            distanceBetweenNodes[i][j] = dist;
        }

        (*node_i).setDistanceMatrix(distanceBetweenNodes);
    }
    // Calcula diagonal inferior.
    for(unsigned long i = 1+1; i < nodesAmount+1; ++i){
        for(unsigned long j = 1; j < i; ++j){
            distanceBetweenNodes[i][j] = distanceBetweenNodes[j][i];
        }
    }
}

Instance::~Instance(){
    for(unsigned long i = 1; i < nodesAmount+1; ++i){
        free(distanceBetweenNodes[i]);
    }
    free(distanceBetweenNodes);
}


void Instance::print(bool /*unused*/) const{
    for(const auto &truck: trucksList){
        truck.print(true);
    }
    printf("\n");

    milkList.print(true);

    for(const auto &node: nodesList){
        (*node).print(true);
    }
    printf("\n");

    printf("      ");
    for(unsigned long i = 1; i < nodesAmount+1; ++i){
        printf("%5lu ", i);
    }
    printf("\n");
    for(unsigned long i = 1; i < nodesAmount+1; ++i){
        printf("%5lu ", i);
        for(unsigned long j = 1; j < nodesAmount+1; ++j){
            printf("%5.1Lf ", distanceBetweenNodes[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}


Node *Instance::getInitialNode() const{
    return nodesList.at(0).get();
}

const std::vector<Truck> &Instance::getTrucksList() const{
    return trucksList;
}
[[nodiscard]]
const MilkTypesList &Instance::getMilkList() const{
    return milkList;
}
[[nodiscard]]
const std::vector<std::unique_ptr<Node>> &Instance::getNodesList() const{
    return nodesList;
}


void Instance::readTrucks(FILE *arch){
    fscanf(arch, "%lu", &trucksAmount);
    assert(trucksAmount != 0);

    trucksList.reserve(trucksAmount);
    for(unsigned long i = 0; i < trucksAmount; ++i){
        long capacity;
        fscanf(arch, "%ld", &capacity);
        trucksList.emplace_back(i+1, capacity);
    }
}

void Instance::readMilk(FILE *arch){
    unsigned long milk_types_amount;
    fscanf(arch, "%lu", &milk_types_amount);
    assert(milk_types_amount != 0);

    std::vector<long> quotas_list;
    quotas_list.reserve(milk_types_amount);
    for(unsigned long i = 0; i < milk_types_amount; ++i){
        long data;
        fscanf(arch, "%ld", &data);
        quotas_list.emplace_back(data);
    }

    std::vector<long double> profit_list;
    profit_list.reserve(milk_types_amount);
    for(unsigned long i = 0; i < milk_types_amount; ++i){
        long double data;
        fscanf(arch, "%Lf", &data);
        profit_list.emplace_back(data);
    }

    milkList.setTotal(milk_types_amount);
    for(unsigned long i = 0; i < milk_types_amount; ++i){
        milkList.add(static_cast<char>(i), quotas_list.at(i), profit_list.at(i));
    }
}

void Instance::readNodes(FILE *arch){
    fscanf(arch, "%lu", &nodesAmount);
    assert(nodesAmount != 0);

    nodesList.reserve(nodesAmount);
    for(unsigned long i = 0; i < nodesAmount; ++i){
        long id, x, y, amountProduced;
        char typeProduced;
        fscanf(arch, "%ld %ld %ld %c %ld", &id, &x, &y, &typeProduced, &amountProduced);
        nodesList.push_back(std::make_unique<Node>(id, x, y, typeProduced, amountProduced));
        if(typeProduced >= 'A') milkList.at(typeProduced).addProducedMilk(amountProduced);
    }
}
