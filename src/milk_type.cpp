#include "milk_type.hpp"

#include <cstdio>


MilkType::MilkType(char type_id, long milk_quota, long double milk_profit)
: profit(milk_profit), quota(milk_quota), typeId(static_cast<char>('A' + type_id)){
}

void MilkType::print(bool newline) const{
    printf("<MilkType. typeId: %c, quota: %ld, profit: %Lf>%s", typeId, quota, profit, newline?"\n":"");
}

char MilkType::getId() const{
    return typeId;
}
long MilkType::getMilkQuota() const{
    return quota;
}
long double MilkType::getMilkProfit() const{
    return profit;
}
long MilkType::getTotalProduced() const{
    return totalProduced;
}

void MilkType::addProducedMilk(long amount){
    totalProduced += amount;
}
