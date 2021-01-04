#include "utils.hpp"

#include <cassert>
#include <cerrno>
#include <cfenv>
#include <cmath>

long double Utils::distance(long double x0, long double y0, long double x1,long double y1){
    long double x = x1-x0;
    long double y = y1-y0;

    long double dist = std::sqrt(x*x + y*y);
    assert(!std::isnan(dist));
    assert(!std::isinf(dist));
    assert(std::isfinite(dist));

    assert(errno != EDOM);
    assert(!std::fetestexcept(FE_INVALID));

    return dist;
}

std::vector<long> Utils::range(long starting_index, long ending_index, long step){
    std::vector<long> vec;
    vec.reserve(ending_index-starting_index);
    for(long i = starting_index; i < ending_index; i+=step){
        vec.emplace_back(i);
    }
    return vec;
}

std::vector<long> Utils::range(unsigned long elements_amount){
    return range(0, (long)elements_amount, 1);
}

bool Utils::debugPrintingEnabled = false;

void Utils::debugPrint(const char *format, ...){
    if(!debugPrintingEnabled){
        return;
    }
    va_list argptr;
    va_start(argptr, format);
    vfprintf(stderr, format, argptr);
    va_end(argptr);
}
