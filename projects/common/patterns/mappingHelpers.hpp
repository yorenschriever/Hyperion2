#pragma once
#include <math.h>
#include "utils.hpp"

float fromTop(float y){
    return (y - 0.1);
}

float fromBottom(float y){
    return 1.0 - fromTop(y);
}

float around(float th){
    return Utils::modulus_f((th + M_PI)/(2*M_PI));
}

inline float softEdge(float dist, float size, float edgeSize = 0.03)
{
    if (dist > size) return 0;
    
    if (dist < size - edgeSize) return 1;
    
    return (size-dist) / edgeSize;
}