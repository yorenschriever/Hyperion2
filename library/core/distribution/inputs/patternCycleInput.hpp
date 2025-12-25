#pragma once

#include "core/generation/patterns/pattern.hpp"
#include "baseInput.hpp"
#include "log.hpp"
#include "utils.hpp"
#include <algorithm>

//PatternCycleInput shows each pattern for a while.
template <class T_COLOUR>
class PatternCycleInput final: public BaseInput
{

public:
    Params params;

    //length = the length in pixels
    //patterns = a list of 8 patterns to attach to each button
    //duration = the time each pattern is displayed
    PatternCycleInput(int length, const std::vector<Pattern<T_COLOUR>*> patterns, int duration)
    {
        this->length = length;
        this->patterns = patterns;
        this->duration = duration;
        this->start = Utils::millis();
    }

    Buffer *getData() override 
    {
        const int bufferSize = length * sizeof(T_COLOUR);
        auto patternBuffer = BufferPool::getBuffer(bufferSize);
        if (!patternBuffer)
        {
            Log::error("PATTERN_CYCLE_INPUT", "Unable to allocate memory for PatternCycleInput, free heap = %d\n", Utils::get_free_heap());
            Utils::exit();
        }
        auto dataPtr = patternBuffer->getData();

        for (int i = 0; i < length; i++)
            dataPtr[i] = T_COLOUR();

        for (int i=0; i < patterns.size(); i++) {
            bool active = ((Utils::millis() - start) / duration) % patterns.size() == i;
            patterns[i]->Calculate(dataPtr, length, active, &params);
        }

        fpsCounter.increaseUsedFrameCount();

        return patternBuffer;
    }

private:
    int length = 0;
    std::vector<Pattern<T_COLOUR>*> patterns;
    unsigned long start;
    unsigned int duration;
};