#include "utils.hpp"

#include <math.h>

double Utils::distance(double x0, double y0, double x1, double y1){
    double x = x1-x0;
    double y = y1-y0;

    return sqrtl(x*x+y*y);
}

int Utils::randomGenerator(int i){
    return std::rand()%i;
}

void Utils::debugPrint(const char *format, ...){
    if(debugPrintingEnabled){
        va_list argptr;
        va_start(argptr, format);
        vfprintf(stderr, format, argptr);
        va_end(argptr);
    }
}
