#pragma once
#ifndef MILK_TYPE_HPP
#define MILK_TYPE_HPP

/**
 *  Modela las cantidades mínimas y las ganancias asociadas a cada tipo de leche.
 */
class MilkType{
public:
    MilkType(char type_id, long milk_quota, long double milk_profit);

    void print(bool newline=false) const;

    [[nodiscard]]
    char getId() const;
    [[nodiscard]]
    long getMilkQuota() const;
    [[nodiscard]]
    long double getMilkProfit() const;
    [[nodiscard]]
    long getTotalProduced() const;

    void addProducedMilk(long amount);

private:
    long double profit; // La ganancia asociada a este tipo de leche.
    long quota; // Cantidad mínima pedida de este tipo de leche.
    long totalProduced = 0;
    char typeId; // Tipo de leche. 'A', 'B', 'C', etc.
};

#endif
