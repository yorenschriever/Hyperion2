#pragma once

#include "core/generation/patterns/pattern.hpp"
#include "baseInput.hpp"
#include "log.hpp"
#include "utils.hpp"
#include <algorithm>

//PatternCycleInput shows each pattern for a while.
template <class T_COLOR>
class PatternCycleInput final: public BaseInput
{

public:
    Params params;

    //length = the length in pixels
    //patterns = a list of 8 patterns to attach to each button
    //duration = the time each pattern is displayed
    PatternCycleInput(int length, const std::vector<Pattern<T_COLOR>*> patterns, int duration)
    {
        this->length = length;
        this->patterns = patterns;
        this->duration = duration;
        this->start = Utils::millis();
    }
    
    virtual bool ready() override
    {
        return true;
    }

    Buffer process() override 
    {
        auto patternBuffer = Buffer(length * sizeof(T_COLOR));
        patternBuffer.clear<T_COLOR>();

        for (int i=0; i < patterns.size(); i++) {
            bool active = ((Utils::millis() - start) / duration) % patterns.size() == i;
            patterns[i]->Calculate(patternBuffer.as<T_COLOR>(), length, active, &params);
        }

        fpsCounter.increaseUsedFrameCount();

        return patternBuffer;
    }

private:
    int length = 0;
    std::vector<Pattern<T_COLOR>*> patterns;
    unsigned long start;
    unsigned int duration;
};