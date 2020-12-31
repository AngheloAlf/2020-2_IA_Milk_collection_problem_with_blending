#include "route.hpp"

#include <stdio.h>
#include <assert.h>
#include <algorithm>


Route::Route(char milk_type, long nodes_amount)
: milkType(milk_type), milkAmount(0){
    nodes.reserve(nodes_amount);
}
Route::Route(const Route &other)
: truckId(other.truckId), capacityLeft(other.capacityLeft), milkType(other.milkType), nodes(other.nodes), milkAmount(other.milkAmount){
}

Route &Route::operator=(const Route &other){
    truckId = other.truckId;
    capacityLeft = other.capacityLeft;
    milkType = other.milkType;
    nodes = other.nodes;
    milkAmount = other.milkAmount;
    return *this;
}

void Route::print(bool newline) const{
    printf("<Route. truckId: %li, milkType: %c, milkAmount: %5li, capacityLeft: %4li, nodes: [", truckId, milkType, milkAmount, capacityLeft);
    if(nodes.size() > 0){
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
long Route::getCapacityLeft() const{
    return capacityLeft;
}
const std::vector<const Node *> &Route::getNodes(){
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

double Route::evaluateRoute(const Node *initial_node, const std::vector<MilkType> &milk_list) const{
    if(capacityLeft < 0){
        return 0;
    }
    double result = 0;
    char current_milk_type = this->milkType;
    auto milk_iter = std::find_if(milk_list.begin(), milk_list.end(), [&current_milk_type](const MilkType &milk){ /*printf("%c %c\n", current_milk_type, milk.id());*/ ; return current_milk_type == milk.getId(); });
    assert(milk_iter != milk_list.end());

    double profit_percentage = (*milk_iter).getMilkProfit();
    long quota = (*milk_iter).getMilkQuota();
    // Si la cuota de la planta procesadora no se cumple, la ruta aporta 0.
    if(milkAmount < quota){
        return 0;
    }

    const Node *prev_node = initial_node;
    for(auto iter = nodes.begin(); iter != nodes.end(); ++iter){
        result += (**iter).getProduced() * profit_percentage;
        result -= (**iter).distanceTo(*prev_node);

        prev_node = *iter;
    }

    result -= (*prev_node).distanceTo(*initial_node);
    return result;
}
