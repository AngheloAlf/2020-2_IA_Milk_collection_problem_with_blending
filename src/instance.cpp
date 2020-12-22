#include "instance.hpp"

#include <stdio.h>
#include <vector>

#include "truck.hpp"
#include "milk_type.hpp"


union milkdata{
    long quot;
    double prof;
};

Instance::Instance(char *filename){
    FILE *arch = fopen(filename, "r");

    /** Trucks **/
    long trucks_amount;
    fscanf(arch, "%ld", &trucks_amount);

    std::vector<Truck> trucks_list;
    trucks_list.reserve(trucks_amount);
    for(long i = 0; i < trucks_amount; ++i){
        long capacity;
        fscanf(arch, "%ld", &capacity);
        trucks_list.emplace_back(capacity);
    }
    /** Trucks: end **/

    /** Milk types **/
    long milk_types_amount;
    fscanf(arch, "%ld", &milk_types_amount);

    std::vector<milkdata> milkdata_list;
    milkdata_list.reserve(milk_types_amount*2);
    for(long i = 0; i < milk_types_amount; ++i){
        union milkdata data;
        fscanf(arch, "%ld", &data.quot);
        milkdata_list.emplace_back(data);
    }
    for(long i = milk_types_amount; i < milk_types_amount*2; ++i){
        union milkdata data;
        fscanf(arch, "%lf", &data.prof);
        milkdata_list.emplace_back(data);
    }

    char milk_type = 'A';
    std::vector<MilkType> milk_list;
    milk_list.reserve(milk_types_amount);
    for(long i = 0; i < milk_types_amount; ++i){
        milk_list.emplace_back(milk_type, milkdata_list.at(i).quot, milkdata_list.at(i+milk_types_amount).prof);
        ++milk_type;
    }
    /** Milk types: end **/


    fclose(arch);

    long i = 0;
    printf("\n");
    for(auto &truck: trucks_list){
        printf("\n%ld - ", i);
        truck.print();
        ++i;
    }
    printf("\n");

    i = 0;
    printf("\n");
    for(auto &milk: milk_list){
        printf("\n%ld - ", i);
        milk.print();
        ++i;
    }
    printf("\n");
    
}
