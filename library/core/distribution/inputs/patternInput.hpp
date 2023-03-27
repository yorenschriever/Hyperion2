#pragma once

#include "core/generation/patterns/pattern.hpp"
#include "input.hpp"
#include "log.hpp"
#include "utils.hpp"
#include <algorithm>

// PatternInput displays 1 pattern continuously.
template <class T_COLOUR>
class PatternInput : public Input
{

public:
    Params params;

    // length = the length in pixels
    // pattern = the pattern to display
    PatternInput(int length, Pattern<T_COLOUR> *pattern)
    {
        this->length = length;
        this->pattern = pattern;
        this->leddata = (T_COLOUR *)malloc(length * sizeof(T_COLOUR));

        if (!this->leddata)
        {
            Log::error("PATTERN_INPUT", "Unable to allocate memory for PatternInput, free heap = %d\n", Utils::get_free_heap());
            Utils::exit();
        }
    }

    void begin() override
    {
        for (int i = 0; i < length; i++)
            leddata[i] = T_COLOUR();

        pattern->Initialize();
    }

    int loadData(uint8_t *dataPtr, unsigned int buffersize) override
    {
        // reset everything to 0
        for (int i = 0; i < length; i++)
            leddata[i] = T_COLOUR();

        pattern->Calculate(leddata, length, true, &params);

        memcpy(dataPtr, leddata, std::min((int)buffersize, (int) (length * sizeof(T_COLOUR))));

        usedframecount++;

        return length * sizeof(T_COLOUR);
    }

private:
    int length = 0;
    T_COLOUR *leddata;
    Pattern<T_COLOUR> *pattern;
};