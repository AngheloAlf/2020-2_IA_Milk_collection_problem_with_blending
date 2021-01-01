#include "milk_type.hpp"

#include <cstdio>


MilkType::MilkType(char typeId, long quota, double profit){
    this->typeId = typeId;
    this->quota = quota;
    this->profit = profit;
}

void MilkType::print(bool newline) const{
    printf("<MilkType. typeId: %c, quota: %ld, profit: %lf>%s", typeId, quota, profit, newline?"\n":"");
}

char MilkType::getId() const{
    return typeId;
}
long MilkType::getMilkQuota() const{
    return quota;
}
double MilkType::getMilkProfit() const{
    return profit;
}
