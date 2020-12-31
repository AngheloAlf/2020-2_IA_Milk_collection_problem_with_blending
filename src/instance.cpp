#include "instance.hpp"

#include <stdlib.h>
#include <assert.h>
#include "utils.hpp"


Instance::Instance(char *filename){
    FILE *arch = fopen(filename, "r");

    readTrucks(arch);
    readMilk(arch);
    readNodes(arch);

    fclose(arch);
    
    distanceBetweenNodes = (double **)malloc(sizeof(double *)*nodesAmount);
    for(long i = 0; i < nodesAmount; ++i){
        distanceBetweenNodes[i] = (double *)malloc(sizeof(double)*nodesAmount);
        Node *node_i = nodesList.at(i).get();
        distanceBetweenNodes[i][i] = 0;

        for(long j = i+1; j < nodesAmount; ++j){
            double dist = (*node_i).distanceTo(*nodesList.at(j).get());
            distanceBetweenNodes[i][j] = dist;
        }
    }
    for(long i = 1; i < nodesAmount; ++i){
        for(long j = 0; j < i; ++j){
            distanceBetweenNodes[i][j] = distanceBetweenNodes[j][i];
        }
    }
}

Instance::~Instance(){
    for(long i = 0; i < nodesAmount; ++i){
        free(distanceBetweenNodes[i]);
    }
    free(distanceBetweenNodes);
}


void Instance::print(bool) const{
    for(auto &truck: trucksList){
        truck.print(true);
    }
    printf("\n");

    for(auto &milk: milkList){
        milk.print(true);
    }
    printf("\n");

    for(auto &node: nodesList){
        (*node).print(true);
    }
    printf("\n");

    for(long i = 0; i < nodesAmount; ++i){
        for(long j = 0; j < nodesAmount; ++j){
            printf("%5.1lf ", distanceBetweenNodes[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}


std::vector<Route> Instance::initialSolution() const{
    std::vector<Route> routes;
    routes.reserve(trucksAmount);
    char milk_type = 'A';
    for(long i = 0; i < trucksAmount; ++i){
        routes.emplace_back(milk_type++, nodesAmount);
    }
    Utils::randomizeVector(routes);
    for(long i = 0; i < trucksAmount; ++i){
        routes.at(i).setTruck(trucksList.at(i));
    }

    std::vector<Node *> farms_list;
    farms_list.reserve(nodesList.size());
    for(auto iter = nodesList.begin()+1; iter < nodesList.end(); ++iter){
        farms_list.push_back((*iter).get());
    }
    Utils::randomizeVector(farms_list);

    long TOL = 0;
    long truck_counter = 0;

    /*print(true);

    for(auto &asdf: farms_list){
        asdf.print(true);
    }*/

    while(farms_list.size() > 0){
        auto selected_farm_iter = Utils::selectRandomly(farms_list);
        assert(selected_farm_iter != farms_list.end());
        Node *selected_farm = *selected_farm_iter;

        for(unsigned long i = 0; i < trucksList.size(); ++i){
            Route &route = routes.at(i);
            if(route.getCapacityLeft() - (*selected_farm).getProduced() >= TOL && route.getMilkType() == (*selected_farm).getQuality()){
                route.addFarm(selected_farm);
                farms_list.erase(selected_farm_iter);
                ++truck_counter;

                if(farms_list.size() == 0) break;

                selected_farm_iter = Utils::selectRandomly(farms_list);
                assert(selected_farm_iter != farms_list.end());
                selected_farm = *selected_farm_iter;
            }
        }

        if(truck_counter == 0){
            TOL -= 10;
            continue;
        }
        else{
            truck_counter = 0;
        }
        
    }

    return routes;
}

double Instance::evaluateSolution(std::vector<Route> &sol) const{
    double result = 0;

    for(Route route: sol){
        result += route.evaluateRoute(nodesList.at(0).get(), milkList);
    }

    return result;
}

std::vector<Route> Instance::hillClimbing(std::vector<Route> &initial_solution, long K) const{
    std::vector<Route> best_solution(initial_solution);
    std::vector<Route> solution(initial_solution);
    double best_quality = evaluateSolution(best_solution);

    for(long i = 0; i < K; ++i){
        bool global_local = true;
        do{
            global_local |= extraLocalSearch(solution);
            //global_local |= intra_local_search();
            //global_local = true;
        } while(!global_local);

        double quality = evaluateSolution(solution);
        if(quality > best_quality){
            best_solution = solution;
            best_quality = quality;
        }
    }
    
    return best_solution;
}


bool Instance::extraLocalSearch(std::vector<Route> &solution) const{
    bool extra_local = false;

    double old_quality = evaluateSolution(solution);
    std::vector<Route> alternative(solution);

    do{
        extra_local = true;
        
        for(auto i = alternative.begin(); i != alternative.end(); ++i){
            Route &src = *i;
            for(auto j = alternative.begin(); j != alternative.end(); ++j){
                if(i == j){
                    continue;
                }
                Route &dst = *j;

                const std::vector<const Node *> &nodes_list_src = src.getNodes();
                const std::vector<const Node *> &nodes_list_dst = dst.getNodes();
                
                for(unsigned long node_i = 0; node_i < nodes_list_src.size(); ++node_i){
                    auto src_iter = nodes_list_src.begin() + node_i;
                    const Node *node_src = *src_iter;

                    // Evitar agregar el nodo a la ruta si esto sobrecarga al camión.
                    if(dst.getCapacityLeft() - (*node_src).getProduced() < 0){
                        continue;
                    }

                    src.removeFarm(src_iter);

                    for(unsigned long node_j = 0; node_j < nodes_list_dst.size(); ++node_j){
                        auto dst_iter = nodes_list_dst.begin();
                        dst_iter += node_j;

                        dst.addFarm(dst_iter, node_src);

                        double new_quality = evaluateSolution(alternative);
                        if(new_quality > old_quality){
                            solution = alternative;
                            return false;
                        }

                        dst_iter = nodes_list_dst.begin() + node_j;
                        dst.removeFarm(dst_iter);
                    }

                    src.addFarm(src_iter, node_src);
                }
            }
        }
    } while(!extra_local);

    return true;
}


void Instance::readTrucks(FILE *arch){
    fscanf(arch, "%ld", &trucksAmount);

    trucksList.reserve(trucksAmount);
    for(long i = 0; i < trucksAmount; ++i){
        long capacity;
        fscanf(arch, "%ld", &capacity);
        trucksList.emplace_back(i+1, capacity);
    }
}

void Instance::readMilk(FILE *arch){
    fscanf(arch, "%ld", &milkTypesAmount);

    std::vector<long> quotas_list;
    quotas_list.reserve(milkTypesAmount);
    for(long i = 0; i < milkTypesAmount; ++i){
        long data;
        fscanf(arch, "%ld", &data);
        quotas_list.emplace_back(data);
    }

    std::vector<double> profit_list;
    profit_list.reserve(milkTypesAmount);
    for(long i = 0; i < milkTypesAmount; ++i){
        double data;
        fscanf(arch, "%lf", &data);
        profit_list.emplace_back(data);
    }

    char milk_type = 'A';
    milkList.reserve(milkTypesAmount);
    for(long i = 0; i < milkTypesAmount; ++i){
        milkList.emplace_back(milk_type, quotas_list.at(i), profit_list.at(i));
        ++milk_type;
    }
}

void Instance::readNodes(FILE *arch){
    fscanf(arch, "%ld", &nodesAmount);

    nodesList.reserve(nodesAmount);
    for(long i = 0; i < nodesAmount; ++i){
        long id, x, y, amountProduced;
        char typeProduced;
        fscanf(arch, "%ld %ld %ld %c %ld", &id, &x, &y, &typeProduced, &amountProduced);
        nodesList.push_back(std::make_unique<Node>(id, x, y, typeProduced, amountProduced));
    }
}

