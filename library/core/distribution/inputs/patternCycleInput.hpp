#pragma once

#include "core/generation/patterns/pattern.hpp"
#include "input.hpp"
#include "log.hpp"
#include "utils.hpp"
#include <algorithm>

//PatternCycleInput shows each pattern for a while.
template <class T_COLOUR>
class PatternCycleInput final: public Input
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
        this->leddata = (T_COLOUR *)malloc(length * sizeof(T_COLOUR));
        this->duration = duration;

        if (!this->leddata)
        {
            Log::error("PATTERN_INPUT", "Unable to allocate memory for PatternCycleInput, free heap = %d\n", Utils::get_free_heap());
            Utils::exit();
        }
    }

    virtual void begin()
    {
        for (int i = 0; i < length; i++)
            leddata[i] = T_COLOUR();

        for (auto pattern:patterns)
            pattern->Initialize();

        start = Utils::millis();
    }

    virtual int loadData(uint8_t *dataPtr, unsigned int buffersize)
    {
        for (int i = 0; i < length; i++)
            leddata[i] = T_COLOUR();

        for (int i=0; i < patterns.size(); i++) {
            bool active = ((Utils::millis() - start) / duration) % patterns.size() == i;
            //active |= ((Utils::millis() - start) / duration) % patterns.size() == (i+1) % patterns.size();
            patterns[i]->Calculate(leddata, length, active, &params);
        }

        memcpy(dataPtr, leddata, std::min((int)buffersize, (int) (length * sizeof(T_COLOUR))));

        usedframecount++;

        return length * sizeof(T_COLOUR);
    }

private:
    int length = 0;
    T_COLOUR *leddata;
    std::vector<Pattern<T_COLOUR>*> patterns;
    unsigned long start;
    unsigned int duration;
};