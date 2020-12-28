#pragma once
#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <algorithm>
#include <random>

/// Calcula la distancia entre 2 puntos.
double distance(double x0, double y0, double x1, double y1);

/// https://stackoverflow.com/a/42484107/6292472
/*template<class T>
void randomizeOrder(std::vector<T> &out, std::vector<T> &in){
    std::sample(
        in.begin(),
        in.end(),
        std::back_inserter(out),
        5,
        std::mt19937{std::random_device{}()}
    );
}*/

int randomGenerator(int i);

template<class T>
void randomizeVector(std::vector<T> &vec){
    std::random_shuffle(vec.begin(), vec.end(), randomGenerator);
}


template<typename T>
typename std::vector<T>::iterator selectRandomly(std::vector<T> &vec){
    if(vec.size() == 0){
        return vec.end();
    }
    auto start = vec.begin();
    auto end = vec.end();
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(gen));
    return start;
}


#endif
