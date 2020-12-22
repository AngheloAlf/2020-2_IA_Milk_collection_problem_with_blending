#pragma once
#ifndef MILK_TYPE_HPP
#define MILK_TYPE_HPP


class MilkType{
private:
    char typeId;
    long quota;
    double profit;

public:
    MilkType(char typeId, long quota, double profit);

    void print();
};



#endif
