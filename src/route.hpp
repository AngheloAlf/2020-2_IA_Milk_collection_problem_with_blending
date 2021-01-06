#pragma once
#ifndef ROUTE_HPP
#define ROUTE_HPP

#include <vector>

#include "truck.hpp"
#include "node.hpp"
#include "milk_types_list.hpp"

class Route{
public:
    Route(const MilkTypesList *milk_list, char milk_type, unsigned long nodes_amount, unsigned long milk_types_amount);

    void print(bool newline=false) const;

    [[nodiscard]]
    long getTruckId() const;
    [[nodiscard]]
    char getMilkType() const;
    [[nodiscard]]
    long getMilkAmount() const;
    [[nodiscard]]
    long getCapacityLeft() const;
    [[nodiscard]]
    const std::vector<const Node *> &getNodes() const;
    [[nodiscard]]
    const MilkType &getMilkTypeInfo() const;

    void setTruck(const Truck &truck);

    /// Agrega la granja a la ruta.
    /// Retorna `false` si al agregar la granja se está violando alguna restricción (capacidad del camión, etc...).
    /// Retorna `true` en caso contrario.
    /// En ambos casos la granja es agregada a la ruta.
    bool addFarm(const Node *farm);
    /// Agrega la granja a la ruta en la posición especificada.
    bool addFarm(long position, const Node *farm);

    /// Remueve la granja de la posición especificada de esta ruta.
    /// Retorna `false` si al remover la granja se está violando alguna restricción (no se cumplen las cuotas, etc...).
    /// Retorna `true` en caso contrario.
    bool removeFarm(long position);

    void reverseFarmsOrder(long left, long right);

    [[nodiscard]]
    bool doesFulfilQuota() const;
    [[nodiscard]]
    bool isFeasible() const;

    [[nodiscard]]
    long double evaluateRoute(const Node *initial_node);
    [[nodiscard]]
    long double calculateTransportCosts(const Node *initial_node) const;
    [[nodiscard]]
    long double calculateMilkProfits() const;

private:
    std::vector<const Node *> nodes;
    std::vector<unsigned long> nodes_counter;
    const MilkTypesList *milkList;
    long double quality = 0;
    long truckId;
    long capacityLeft;
    long milkAmount;
    char milkType;
    bool changed = true;

    [[nodiscard]]
    char recalculateMilkType() const;
};

#endif
