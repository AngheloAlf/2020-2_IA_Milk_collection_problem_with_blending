#include "instance.hpp"

#include <stdio.h>
#include <vector>

#include "truck.hpp"

Instance::Instance(char *filename){
    FILE *arch = fopen(filename, "r");

    long trucks_amount;
    fscanf(arch, "%ld", &trucks_amount);

    std::vector<Truck> trucks_list;
    trucks_list.reserve(trucks_amount);
    for(long i = 0; i < trucks_amount; ++i){
        long capacity;
        fscanf(arch, "%ld", &capacity);
        trucks_list.emplace_back(capacity);
    }


    fclose(arch);

    long i = 0;
    for(auto &truck: trucks_list){
        printf("%ld - ", i);
        truck.print();
        printf("\n");
        ++i;
    }
    
}
