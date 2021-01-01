#pragma once
#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <algorithm>
#include <random>

namespace Utils{
    /// Calcula la distancia entre 2 puntos.
    [[nodiscard]]
    long double distance(long double x0, long double y0, long double x1, long double y1);


    int randomGenerator(int i);

    template<class T>
    void randomizeVector(std::vector<T> &vec){
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::shuffle(vec.begin(), vec.end(), gen);
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

    static bool debugPrintingEnabled = false;
    void debugPrint(const char *format, ...);
};

#endif
