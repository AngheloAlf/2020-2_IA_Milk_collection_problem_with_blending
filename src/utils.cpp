#include "utils.hpp"

#include <cmath>

long double Utils::distance(long double x0, long double y0, long double x1,long double y1){
    long double x = x1-x0;
    long double y = y1-y0;

    return sqrtl(x*x+y*y);
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
