#pragma once

#include "core/generation/patterns/pattern.hpp"
#include "baseInput.hpp"
#include "log.hpp"
#include "utils.hpp"
#include <algorithm>

// PatternInput displays 1 pattern continuously.
template <class T_COLOUR>
class PatternInput final: public BaseInput
{

public:
    Params params;

    // length = the length in pixels
    // pattern = the pattern to display
    PatternInput(int length, Pattern<T_COLOUR> *pattern)
    {
        this->length = length;
        this->pattern = pattern;
    }

    Buffer *getData() override
    {
        auto patternBuffer = BufferPool::getBuffer(length * sizeof(T_COLOUR));
        if (!patternBuffer)
        {
            Log::error("PATTERN_INPUT", "Unable to allocate memory for PatternInput, free heap = %d\n", Utils::get_free_heap());
            Utils::exit();
        }
        auto ledData = (T_COLOUR *)patternBuffer->getData();

        for (int i = 0; i < length; i++)
            ledData[i] = T_COLOUR();

        pattern->Calculate(ledData, length, true, &params);

        fpsCounter.increaseUsedFrameCount();

        return patternBuffer;
    }

private:
    int length = 0;
    T_COLOUR *ledData;
    Pattern<T_COLOUR> *pattern;
};