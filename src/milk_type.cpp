#include "milk_type.hpp"

#include <stdio.h>


MilkType::MilkType(char typeId, long quota, double profit){
    this->typeId = typeId;
    this->quota = quota;
    this->profit = profit;
}

char MilkType::id(){
    return typeId;
}
long MilkType::milkQuota(){
    return quota;
}
double MilkType::milkProfit(){
    return profit;
}

void MilkType::print(){
    printf("<MilkType. typeId: %c, quota: %ld, profit: %lf>", typeId, quota, profit);
}
