#pragma once
#include <math.h>
#include "generation/pixelMap.hpp"
#include "utils.hpp"

//helper functions to map the bottom (z=-0.45) - top (z=0.45) to range 0-1
float fromTop(float y){
    return (y + 0.1)/1.1;
}

float fromBottom(float y){
    return 1.0 - fromTop(y);
}

float around(float th){
    return (th + M_PI)/(2*M_PI);
}

float fromMid(PixelMap3d::CylindricalPixelPosition pos)
{
    if (pos.z >= 0.44)
        //ledster, bovenin
        return Utils::rescale(pos.r,0,1,0.4,1);    
    else
        //staand
        return Utils::rescale(pos.z,0,1,0,.45);
}

inline float softEdge(float dist, float size, float edgeSize = 0.03)
{
    if (dist > size) return 0;
    
    if (dist < size - edgeSize) return 1;
    
    return (size-dist) / edgeSize;
}