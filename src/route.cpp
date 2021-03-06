#include "route.hpp"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cmath>

Route::Route(const MilkTypesList *milk_list, char milk_type, unsigned long nodes_amount, unsigned long milk_types_amount)
: milkList(milk_list), truckId(0), capacityLeft(0), milkAmount(0), milkType(milk_type){
    nodes.reserve(nodes_amount);
    nodesCounter.assign(milk_types_amount, 0);
}

void Route::print(bool newline) const{
    printf("<Route. truckId: %li, milkType: %c, milkQuota: %ld, milkAmount: %5ld, capacityLeft: %+5li, nodes: [", truckId, milkType, getMilkTypeInfo().getMilkQuota(), milkAmount, capacityLeft);
    if(!nodes.empty()){
        printf("%2li", nodes.at(0)->getId());
        for(unsigned long i = 1; i < nodes.size(); ++i){
            printf(", %2li", nodes.at(i)->getId());
        }
    }
    printf("]");
    if(!changed){
        printf(", quality: %.3Lf", quality);
    }
    printf(">%s", newline? "\n" : "");
}

long Route::getTruckId() const{
    return truckId;
}
char Route::getMilkType() const{
    return milkType;
}
long Route::getMilkAmount() const{
    return milkAmount;
}
long Route::getCapacityLeft() const{
    return capacityLeft;
}
const std::vector<const Node *> &Route::getNodes() const{
    return nodes;
}
const MilkType &Route::getMilkTypeInfo() const{
    const auto &current_milk_type = (*milkList).at(this->milkType);
    assert(current_milk_type.getId() == this->milkType);

    return current_milk_type;
}

void Route::setTruck(const Truck &truck){
    this->truckId = truck.getId();
    this->capacityLeft = truck.getCapacity();
}

bool Route::isFarmMilkCompatibleWithCurrentMilkType(const Node *farm) const{
    return (*farm).getQuality() <= milkType;
}

bool Route::canAddFarm(const Node *farm) const{
    assert((*farm).getQuality() != '-');
    // Restricción: Al mezclar la leche su calidad queda como la mas baja entre las mezcladas.
    if((*farm).getQuality() > milkType){
        return false;
    }
    // Restricción: No se debe superar la capacidad máxima de los camiones.
    return capacityLeft - (*farm).getProduced() >= 0;
}

bool Route::canRemoveFarm(long position) const{
    assert(position >= 0);
    assert((unsigned long)position < nodes.size());

    const Node *node = nodes.at(position);

    if(nodesCounter[(*node).getQuality()-'A'] == 1){
        return false;
    }

    // Restricción: El total de la leche debe superar la cuota mínima de la planta procesadora.
    long quota = getMilkTypeInfo().getMilkQuota();
    return (milkAmount - (*node).getProduced() >= quota);
}

void Route::addFarm(const Node *farm){
    assert((*farm).getQuality() != '-');

    addFarm_updateValues(farm);

    nodes.push_back(farm);
    changed = true;
}
void Route::addFarm(long position, const Node *farm){
    assert(position >= 0);
    assert((unsigned long)position <= nodes.size());
    assert((*farm).getQuality() != '-');

    addFarm_updateValues(farm);

    nodes.insert(nodes.begin() + position, farm);

    changed = true;
}

void Route::removeFarm(long position){
    assert(!nodes.empty());
    assert(position >= 0);
    assert((unsigned long)position < nodes.size());

    removeFarm_updateValues(position);

    nodes.erase(nodes.begin() + position);
    changed = true;
}
void Route::reverseFarmsOrder(long left, long right){
    assert(left >= 0);
    assert(static_cast<unsigned long>(right) < nodes.size());
    assert(left < right);

    std::reverse(nodes.begin() + left, nodes.begin() + right);
    changed = true;
}

void Route::setFarm(long position, const Node *farm){
    assert(position >= 0);
    assert((unsigned long)position < nodes.size());
    assert((*farm).getQuality() != '-');

    removeFarm_updateValues(position);
    addFarm_updateValues(farm);

    nodes[position] = farm;

    changed = true;
}

bool Route::doesFulfilQuota() const{
    return milkAmount >= getMilkTypeInfo().getMilkQuota();
}
bool Route::isFeasible() const{
    if(capacityLeft < 0){
        return false;
    }
    if(!doesFulfilQuota()){
        return false;
    }
    return true;
}

long double Route::evaluateRoute(const Node *initial_node){

    // Si la ruta no ha cambiado, evitamos recalcular y usamos el valor almacenado.
    if(!changed){
        return quality;
    }
    long double distance_penalty = 0;
    long double profit_percentage = getMilkTypeInfo().getMilkProfit();

    // Restricción: Un camión sale de la planta procesadora y vuelve a ella al final del día.
    const Node *prev_node = initial_node;
    for(const auto &node_ptr: nodes){
        distance_penalty += (*node_ptr).cachedDistance(*prev_node);
        prev_node = node_ptr;
    }
    distance_penalty += (*prev_node).cachedDistance(*initial_node);

    changed = false;
    quality = milkAmount * profit_percentage - distance_penalty;

    // Penalizar la calidad de la ruta si es infactible.
    if(capacityLeft < 0){
        quality += capacityLeft;
    }
    if(!doesFulfilQuota()){
        quality -= getMilkTypeInfo().getMilkQuota() - milkAmount;
    }

    //assert(!std::isnan(quality));
    //assert(!std::isinf(quality));
    //assert(std::isfinite(quality));

    return quality;
}

long double Route::calculateTransportCosts(const Node *initial_node) const{
    long double distance_penalty = 0;

    const Node *prev_node = initial_node;
    for(const auto &node: nodes){
        distance_penalty += (*node).cachedDistance(*prev_node);
        prev_node = node;
    }

    distance_penalty += (*prev_node).cachedDistance(*initial_node);

    //assert(!std::isnan(distance_penalty));
    //assert(!std::isinf(distance_penalty));
    //assert(std::isfinite(distance_penalty));

    return distance_penalty;
}

long double Route::calculateMilkProfits() const{
    long double profit_percentage = getMilkTypeInfo().getMilkProfit();
    long double result = milkAmount * profit_percentage;

    //assert(!std::isnan(result));
    //assert(!std::isinf(result));
    //assert(std::isfinite(result));
    return result;
}

char Route::recalculateMilkType() const{
    char i = milkType;
    for(--i; i >= 'A'; --i){
        if(nodesCounter[i-'A'] > 0){
            return i;
            break;
        }
    }
    return 0;
}

void Route::addFarm_updateValues(const Node *farm){
    capacityLeft -= (*farm).getProduced();
    milkAmount += (*farm).getProduced();

    nodesCounter[(*farm).getQuality()-'A'] += 1;

    // Restricción: Al mezclar la leche su calidad queda como la mas baja entre las mezcladas.
    if((*farm).getQuality() > milkType){
        milkType = (*farm).getQuality();
    }
}
void Route::removeFarm_updateValues(long position){
    const Node *node = nodes.at(position);

    capacityLeft += (*node).getProduced();
    milkAmount -= (*node).getProduced();

    nodesCounter[(*node).getQuality()-'A'] -= 1;

    // Si la calidad de la leche de esa granja es la peor, es posible que el conjunto mejore al removerla.
    if((*node).getQuality() == milkType && nodesCounter[(*node).getQuality()-'A'] == 0){
        milkType = recalculateMilkType();
    }
}
