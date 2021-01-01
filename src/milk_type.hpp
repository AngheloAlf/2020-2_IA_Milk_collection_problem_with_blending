#pragma once
#ifndef MILK_TYPE_HPP
#define MILK_TYPE_HPP

/**
 *  Modela las cantidades mínimas y las ganancias asociadas a cada tipo de leche.
 */
class MilkType{
public:
    MilkType(char typeId, long quota, long double profit);

    void print(bool newline=false) const;

    [[nodiscard]]
    char getId() const;
    [[nodiscard]]
    long getMilkQuota() const;
    [[nodiscard]]
    long double getMilkProfit() const;

private:
    char typeId; // Tipo de leche. 'A', 'B', 'C', etc.
    long quota; // Cantidad mínima pedida de este tipo de leche.
    long double profit; // La ganancia asociada a este tipo de leche.
};



#endif
