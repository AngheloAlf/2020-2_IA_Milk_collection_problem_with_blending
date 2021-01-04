#include "route.hpp"

#include <cstdio>
#include <cassert>
#include <algorithm>


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


const MilkType &findMilkType(const std::vector<MilkType> &milk_list, char current_milk_type){
    auto milk_iter = std::find_if(milk_list.begin(), milk_list.end(), [&current_milk_type](const MilkType &milk){ return current_milk_type == milk.getId(); });
    assert(milk_iter != milk_list.end());
    return *milk_iter;
}

long double Route::evaluateRoute(const Node *initial_node, const std::vector<MilkType> &milk_list){
    if(capacityLeft < 0){
        return 0;
    }
    if(!changed){
        return quality;
    }
    long milk_produced = 0;
    long double distance_penalty = 0;

    auto current_milk_type = findMilkType(milk_list, this->milkType);

    long double profit_percentage = current_milk_type.getMilkProfit();
    long quota = current_milk_type.getMilkQuota();
    // Si la cuota de la planta procesadora no se cumple, la ruta aporta 0.
    if(milkAmount < quota){
        return 0;
    }

    const Node *prev_node = initial_node;
    for(const auto &node_ptr: nodes){
        const auto &node = *node_ptr;
        milk_produced += node.getProduced();
        distance_penalty += node.cachedDistance(*prev_node);

        prev_node = node_ptr;
    }

    distance_penalty += (*prev_node).cachedDistance(*initial_node);

    changed = false;
    quality = milk_produced * profit_percentage - distance_penalty;
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
    return distance_penalty;
}

long double Route::calculateMilkProfits(const std::vector<MilkType> &milk_list) const{
    long milk_produced = 0;

    auto current_milk_type = findMilkType(milk_list, this->milkType);
    long double profit_percentage = current_milk_type.getMilkProfit();
    for(const auto &node: nodes){
        milk_produced += (*node).getProduced();
    }

    return milk_produced * profit_percentage;
}
