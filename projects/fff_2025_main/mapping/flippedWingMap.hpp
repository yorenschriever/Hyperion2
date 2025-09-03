#pragma once
#include "generation/pixelMap.hpp"
#include <algorithm>

//flip all the bars individually
PixelMap flipWingMap(PixelMap wingMap)
{
    PixelMap flippedWingMap;

    for(int i=0; i<wingMap.size(); i++){
        int indexInBar = i % 60;
        int barNumber = i / 60;
        int flippedIndex = (barNumber + 1) * 60 - 1 - indexInBar;
        // flippedWingMap.push_back({.x = -wingMap[flippedIndex].x, .y = wingMap[flippedIndex].y});
        flippedWingMap.push_back(wingMap[flippedIndex]);
    }

    return flippedWingMap;
}