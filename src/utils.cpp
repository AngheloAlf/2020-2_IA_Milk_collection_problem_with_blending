#include "utils.hpp"

#include <math.h>

double distance(double x0, double y0, double x1, double y1){
    double x = x1-x0;
    double y = y1-y0;

    return sqrtl(x*x+y*y);
}

int randomGenerator(int i){
    return std::rand()%i;
}
