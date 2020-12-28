#include "instance.hpp"

#include <stdlib.h>
#include "utils.hpp"


Instance::Instance(char *filename){
    FILE *arch = fopen(filename, "r");

    readTrucks(arch);
    readMilk(arch);
    readNodes(arch);

    fclose(arch);
    
    distanceBetweenNodes = (double **)malloc(sizeof(double *)*nodesAmount);
    for(long i = 0; i < nodesAmount; ++i){
        distanceBetweenNodes[i] = (double *)malloc(sizeof(double)*nodesAmount);
        Node &node_i = nodesList.at(i);
        distanceBetweenNodes[i][i] = 0;

        for(long j = i+1; j < nodesAmount; ++j){
            double dist = node_i.distanceTo(nodesList.at(j));
            distanceBetweenNodes[i][j] = dist;
        }
    }
    for(long i = 1; i < nodesAmount; ++i){
        for(long j = 0; j < i; ++j){
            distanceBetweenNodes[i][j] = distanceBetweenNodes[j][i];
        }
    }
}

Instance::~Instance(){
    for(long i = 0; i < nodesAmount; ++i){
        free(distanceBetweenNodes[i]);
    }
    free(distanceBetweenNodes);
}

std::vector<std::vector<long>> Instance::initialSolution(){
    std::vector<std::vector<long>> routes;
    routes.assign(trucksAmount, std::vector<long>());
    for(auto &route: routes){
        route.reserve(nodesAmount);
    }

    std::vector<Node> farms_list(nodesList.begin()+1, nodesList.end());
    //std::random_shuffle(farms_list.begin(), farms_list.end());
    randomizeVector(farms_list);
    //farms_list.reserve(nodesAmount);
    //randomizeOrder(farms_list, nodesList);

    long TOL = 0;
    long truck_counter = 0;

    /*print();
    printf("\n");

    for(auto &asdf: farms_list){
        asdf.print();
        printf("\n");
    }*/

    while(farms_list.size() > 0){
        auto selected_farm_iter = selectRandomly(farms_list);
        if(selected_farm_iter == farms_list.end()){
            break;
        }
        Node selected_farm = *selected_farm_iter;
        
        //selected_farm.print();
        //break;

        //for(Truck &truck: trucksList){
        for(unsigned long i = 0; i < trucksList.size(); ++i){
            Truck &truck = trucksList.at(i);
            if(truck.capacity() - selected_farm.produced() >= TOL /* && milk_quality_solution == selected_farm.quality()*/){
                routes.at(i).push_back(selected_farm.id());
                farms_list.erase(selected_farm_iter);
                ++truck_counter;

                selected_farm_iter = selectRandomly(farms_list);
                if(selected_farm_iter == farms_list.end()){
                    break;
                }
                selected_farm = *selected_farm_iter;
            }
        }

        if(truck_counter == 0){
            TOL -= 10;
            continue;
        }
        else{
            truck_counter = 0;
        }
        
    }

    return routes;
}

void Instance::print(){
    for(auto &truck: trucksList){
        truck.print();
        printf("\n");
    }
    printf("\n");

    for(auto &milk: milkList){
        milk.print();
        printf("\n");
    }
    printf("\n");

    for(auto &node: nodesList){
        node.print();
        printf("\n");
    }
    printf("\n");

    for(long i = 0; i < nodesAmount; ++i){
        for(long j = 0; j < nodesAmount; ++j){
            printf("%5.1lf ", distanceBetweenNodes[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}


void Instance::readTrucks(FILE *arch){
    fscanf(arch, "%ld", &trucksAmount);

    trucksList.reserve(trucksAmount);
    for(long i = 0; i < trucksAmount; ++i){
        long capacity;
        fscanf(arch, "%ld", &capacity);
        trucksList.emplace_back(capacity);
    }
}

void Instance::readMilk(FILE *arch){
    fscanf(arch, "%ld", &milkTypesAmount);

    std::vector<long> quotas_list;
    quotas_list.reserve(milkTypesAmount);
    for(long i = 0; i < milkTypesAmount; ++i){
        long data;
        fscanf(arch, "%ld", &data);
        quotas_list.emplace_back(data);
    }

    std::vector<double> profit_list;
    profit_list.reserve(milkTypesAmount);
    for(long i = 0; i < milkTypesAmount; ++i){
        double data;
        fscanf(arch, "%lf", &data);
        profit_list.emplace_back(data);
    }

    char milk_type = 'A';
    milkList.reserve(milkTypesAmount);
    for(long i = 0; i < milkTypesAmount; ++i){
        milkList.emplace_back(milk_type, quotas_list.at(i), profit_list.at(i));
        ++milk_type;
    }
}

void Instance::readNodes(FILE *arch){
    fscanf(arch, "%ld", &nodesAmount);

    nodesList.reserve(nodesAmount);
    for(long i = 0; i < nodesAmount; ++i){
        long id, x, y, amountProduced;
        char typeProduced;
        fscanf(arch, "%ld %ld %ld %c %ld", &id, &x, &y, &typeProduced, &amountProduced);
        nodesList.emplace_back(id, x, y, typeProduced, amountProduced);
    }
}

