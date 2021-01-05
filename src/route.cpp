#include "route.hpp"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cmath>

Route::Route(char milk_type, unsigned long nodes_amount, unsigned long milk_types_amount)
: truckId(0), capacityLeft(0), milkAmount(0), milkType(milk_type){
    nodes.reserve(nodes_amount);
    nodes_counter.assign(milk_types_amount, 0);
}

void Route::print(bool newline) const{
    printf("<Route. truckId: %li, milkType: %c, milkAmount: %5li, capacityLeft: %4li, nodes: [", truckId, milkType, milkAmount, capacityLeft);
    if(!nodes.empty()){
        printf("%2li", nodes.at(0)->getId());
        for(unsigned long i = 1; i < nodes.size(); ++i){
            printf(", %2li", nodes.at(i)->getId());
        }
    }
    printf("]>%s", newline? "\n" : "");
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

void Route::setTruck(const Truck &truck){
    this->truckId = truck.getId();
    this->capacityLeft = truck.getCapacity();
}
void Route::addFarm(const Node *farm){
    assert((*farm).getQuality() != '-');
    nodes.push_back(farm);
    capacityLeft -= (*farm).getProduced();
    milkAmount += (*farm).getProduced();

    nodes_counter[(*farm).getQuality()-'A'] += 1;

    // Si la calidad de la leche de esta granja es peor, el conjunto de esta ruta empeora.
    if((*farm).getQuality() > milkType){
        milkType = (*farm).getQuality();
    }
    changed = true;
}
void Route::addFarm(long position, const Node *farm){
    assert(position >= 0);
    assert((unsigned long)position <= nodes.size());
    assert((*farm).getQuality() != '-');

    auto iter = nodes.begin() + position;
    nodes.insert(iter, farm);
    capacityLeft -= (*farm).getProduced();
    milkAmount += (*farm).getProduced();

    nodes_counter[(*farm).getQuality()-'A'] += 1;

    // Si la calidad de la leche de esta granja es peor, el conjunto de esta ruta empeora.
    if((*farm).getQuality() > milkType){
        milkType = (*farm).getQuality();
    }
    changed = true;
}
void Route::removeFarm(long position){
    assert(position >= 0);
    assert((unsigned long)position < nodes.size());

    auto iter = nodes.begin() + position;

    capacityLeft += (**iter).getProduced();
    milkAmount -= (**iter).getProduced();

    nodes_counter[(**iter).getQuality()-'A'] -= 1;

    // Si la calidad de la leche de esa granja es la peor es posible que el conjunto mejore al removerla.
    if((**iter).getQuality() == milkType && nodes_counter[(**iter).getQuality()-'A'] == 0){
        char i = milkType;
        milkType = 0;
        for(--i; i >= 'A'; --i){
            if(nodes_counter[i-'A'] > 0){
                milkType = i;
                break;
            }
        }
    }

    nodes.erase(iter);
    changed = true;
}
void Route::reverseFarmsOrder(long left, long right){
    std::reverse(nodes.begin() + left, nodes.begin() + right);
    changed = true;
}


long double Route::evaluateRoute(const Node *initial_node, const std::vector<MilkType> &milk_list){
    if(capacityLeft < 0){
        return 0;
    }
    if(!changed){
        return quality;
    }
    long double distance_penalty = 0;

    const auto &current_milk_type = milk_list.at(this->milkType - 'A');
    assert(current_milk_type.getId() == this->milkType);

    long double profit_percentage = current_milk_type.getMilkProfit();
    long quota = current_milk_type.getMilkQuota();
    // Si la cuota de la planta procesadora no se cumple, la ruta aporta 0.
    if(milkAmount < quota){
        return 0;
    }

    const Node *prev_node = initial_node;
    for(const auto &node_ptr: nodes){
        distance_penalty += (*node_ptr).cachedDistance(*prev_node);
        prev_node = node_ptr;
    }
    distance_penalty += (*prev_node).cachedDistance(*initial_node);

    changed = false;
    quality = milkAmount * profit_percentage - distance_penalty;
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

long double Route::calculateMilkProfits(const std::vector<MilkType> &milk_list) const{
    const auto &current_milk_type = milk_list.at(this->milkType - 'A');
    assert(current_milk_type.getId() == this->milkType);

    long double profit_percentage = current_milk_type.getMilkProfit();
    long double result = milkAmount * profit_percentage;

    //assert(!std::isnan(result));
    //assert(!std::isinf(result));
    //assert(std::isfinite(result));
    return result;
}
