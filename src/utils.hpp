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

    // https://en.cppreference.com/w/cpp/algorithm/random_shuffle
    template<class T>
    void randomizeVector(std::vector<T> &vec){
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::shuffle(vec.begin(), vec.end(), gen);
    }

    // https://stackoverflow.com/a/16421677/6292472
    template<typename T>
    [[nodiscard]]
    typename std::vector<T>::iterator selectRandomly(std::vector<T> &vec){
        if(vec.empty()){
            return vec.end();
        }
        auto start = vec.begin();
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<long> dis(0, vec.size()-1);
        std::advance(start, dis(gen));
        return start;
    }

    [[nodiscard]]
    std::vector<long> range(long starting_index, long ending_index, long step=1);

    [[nodiscard]]
    std::vector<long> range(unsigned long elements_amount);

    extern bool debugPrintingEnabled;
    void debugPrint(const char *format, ...);

    void resetColors();
    void fgColor(int color);
    void bgColor(int color);
    void fgRed();
}

#endif
