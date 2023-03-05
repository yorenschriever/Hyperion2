#pragma once

#include <vector>

class LedShape {
public: 
    int numLedsPerSet;
    int numSets;
    int totalNumLeds;

    LedShape(int numLedsPerSet,int numSets){
        this->numLedsPerSet = numLedsPerSet;
        this->numSets = numSets;
        this->totalNumLeds = numLedsPerSet * numSets;
    }
};

typedef std::vector<LedShape> Installation;