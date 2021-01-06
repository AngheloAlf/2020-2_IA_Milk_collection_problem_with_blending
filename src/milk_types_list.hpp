#pragma once
#ifndef MILK_TYPES_LIST_HPP
#define MILK_TYPES_LIST_HPP

#include <vector>
#include "milk_type.hpp"

/**
 *  Modela las cantidades mínimas y las ganancias asociadas a cada tipo de leche.
 */
class MilkTypesList{
public:
    MilkTypesList() = default;

    void print(bool newline=false) const;

    [[nodiscard]]
    unsigned long getTotal() const;

    [[nodiscard]]
    MilkType &at(char milk_type_id);
    [[nodiscard]]
    const MilkType &at(char milk_type_id) const;

    [[nodiscard]]
    std::vector<MilkType>::iterator begin();
    [[nodiscard]]
    std::vector<MilkType>::const_iterator begin() const;
    [[nodiscard]]
    std::vector<MilkType>::iterator end();
    [[nodiscard]]
    std::vector<MilkType>::const_iterator end() const;

    void setTotal(unsigned long milk_types_amount);

    void add(char type_id, long milk_quota, long double milk_profit);

private:
    unsigned long milkTypesAmount = 0;
    std::vector<MilkType> milkList;
};

#endif
