#include "instance.hpp"


Instance::Instance(char *filename){
    FILE *arch = fopen(filename, "r");

    readTrucks(arch);
    readMilk(arch);
    readNodes(arch);

    fclose(arch);
    
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
    for(long i = milkTypesAmount; i < milkTypesAmount*2; ++i){
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

