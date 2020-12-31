#pragma once
#ifndef MILK_TYPE_HPP
#define MILK_TYPE_HPP

/**
 *  Modela las cantidades mínimas y las ganancias asociadas a cada tipo de leche.
 */
class MilkType{
private:
    char typeId; // Tipo de leche. 'A', 'B', 'C', etc.
    long quota; // Cantidad mínima pedida de este tipo de leche.
    double profit; // La ganancia asociada a este tipo de leche.

public:
    MilkType(char typeId, long quota, double profit);

    char id() const;
    long milkQuota() const;
    double milkProfit() const;

    void print(bool newline=false) const;
};



#endif
