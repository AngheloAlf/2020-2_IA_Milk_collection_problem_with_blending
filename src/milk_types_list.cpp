#include "milk_types_list.hpp"

#include <cassert>
#include <cstdio>


void MilkTypesList::print(bool newline) const{
    for(const auto &milk: milkList){
        milk.print(true);
    }
    if(newline) printf("\n");
}

unsigned long MilkTypesList::getTotal() const{
    return milkTypesAmount;
}


MilkType &MilkTypesList::at(char milk_type_id){
    assert(milk_type_id >= 'A');
    return milkList.at(milk_type_id - 'A');
}
const MilkType &MilkTypesList::at(char milk_type_id) const{
    assert(milk_type_id >= 'A');
    return milkList.at(milk_type_id - 'A');
}


std::vector<MilkType>::iterator MilkTypesList::begin(){
    return milkList.begin();
}
std::vector<MilkType>::const_iterator MilkTypesList::begin() const{
    return milkList.begin();
}
std::vector<MilkType>::iterator MilkTypesList::end(){
    return milkList.end();
}
std::vector<MilkType>::const_iterator MilkTypesList::end() const{
    return milkList.end();
}


void MilkTypesList::setTotal(unsigned long milk_types_amount){
    milkTypesAmount = milk_types_amount;
    milkList.reserve(milk_types_amount);
}

void MilkTypesList::add(char type_id, long milk_quota, long double milk_profit){
    assert(milkTypesAmount != 0);
    milkList.emplace_back(type_id, milk_quota, milk_profit);
}
