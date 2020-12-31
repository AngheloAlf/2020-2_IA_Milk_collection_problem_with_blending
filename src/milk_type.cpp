#include "milk_type.hpp"

#include <stdio.h>


MilkType::MilkType(char typeId, long quota, double profit){
    this->typeId = typeId;
    this->quota = quota;
    this->profit = profit;
}

char MilkType::id() const{
    return typeId;
}
long MilkType::milkQuota() const{
    return quota;
}
double MilkType::milkProfit() const{
    return profit;
}

void MilkType::print(bool newline) const{
    printf("<MilkType. typeId: %c, quota: %ld, profit: %lf>%s", typeId, quota, profit, newline?"\n":"");
}
