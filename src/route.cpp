#include "route.hpp"

#include <cstdio>
#include <cassert>
#include <algorithm>


Route::Route(char milk_type, unsigned long nodes_amount)
: truckId(0), capacityLeft(0), milkAmount(0), milkType(milk_type){
    nodes.reserve(nodes_amount);
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
    nodes.push_back(farm);
    capacityLeft -= (*farm).getProduced();
    milkAmount += (*farm).getProduced();

    // Si la calidad de la leche de esta granja es peor, el conjunto de esta ruta empeora.
    if((*farm).getQuality() > milkType){
        milkType = (*farm).getQuality();
    }
}
void Route::addFarm(std::vector<const Node *>::const_iterator &position, const Node *farm){
    nodes.insert(position, farm);
    capacityLeft -= (*farm).getProduced();
    milkAmount += (*farm).getProduced();

    // Si la calidad de la leche de esta granja es peor, el conjunto de esta ruta empeora.
    if((*farm).getQuality() > milkType){
        milkType = (*farm).getQuality();
    }
}
void Route::removeFarm(std::vector<const Node *>::const_iterator &position){
    capacityLeft += (**position).getProduced();
    milkAmount -= (**position).getProduced();
    // Si la calidad de la leche de esa granja es la peor es posible que el conjunto mejore al removerla.
    if((**position).getQuality() == milkType){
        milkType = 0;
        for(auto &node: nodes){
            if((*node).getQuality() > milkType){
                milkType = (*node).getQuality();
            }
        }
    }

    nodes.erase(position);
}
void Route::reverseFarmsOrder(std::vector<const Node *>::difference_type left, std::vector<const Node *>::difference_type right){
    std::reverse(nodes.begin() + left, nodes.begin() + right);
}


const MilkType &findMilkType(const std::vector<MilkType> &milk_list, char current_milk_type){
    auto milk_iter = std::find_if(milk_list.begin(), milk_list.end(), [&current_milk_type](const MilkType &milk){ return current_milk_type == milk.getId(); });
    assert(milk_iter != milk_list.end());
    return *milk_iter;
}

long double Route::evaluateRoute(const Node *initial_node, const std::vector<MilkType> &milk_list) const{
    if(capacityLeft < 0){
        return 0;
    }
    long double result = 0;

    auto current_milk_type = findMilkType(milk_list, this->milkType);

    long double profit_percentage = current_milk_type.getMilkProfit();
    long quota = current_milk_type.getMilkQuota();
    // Si la cuota de la planta procesadora no se cumple, la ruta aporta 0.
    if(milkAmount < quota){
        return 0;
    }

    const Node *prev_node = initial_node;
    for(const auto &node: nodes){
        result += (*node).getProduced() * profit_percentage;
        result -= (*node).distanceTo(*prev_node);

        prev_node = node;
    }

    result -= (*prev_node).distanceTo(*initial_node);
    return result;
}

long double Route::calculateTransportCosts(const Node *initial_node) const{
    long double result = 0;

    const Node *prev_node = initial_node;
    for(const auto &node: nodes){
        result += (*node).distanceTo(*prev_node);

        prev_node = node;
    }

    result += (*prev_node).distanceTo(*initial_node);
    return result;
}

long double Route::calculateMilkProfits(const std::vector<MilkType> &milk_list) const{
    long double result = 0;

    auto current_milk_type = findMilkType(milk_list, this->milkType);

    long double profit_percentage = current_milk_type.getMilkProfit();
    for(const auto &node: nodes){
        result += (*node).getProduced() * profit_percentage;
    }

    return result;
}
