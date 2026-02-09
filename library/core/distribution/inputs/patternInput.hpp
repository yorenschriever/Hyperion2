#pragma once

#include "core/generation/patterns/pattern.hpp"
#include "baseInput.hpp"
#include "log.hpp"
#include "utils.hpp"
#include <algorithm>

// PatternInput displays 1 pattern continuously.
template <class T_COLOR>
class PatternInput final: public BaseInput
{

public:
    Params params;

    // length = the length in pixels
    // pattern = the pattern to display
    PatternInput(int length, Pattern<T_COLOR> *pattern)
    {
        this->length = length;
        this->pattern = pattern;
    }

    virtual bool ready() override
    {
        return true;
    }

    Buffer process() override
    {
        auto patternBuffer = Buffer(length * sizeof(T_COLOR));
        patternBuffer.clear<T_COLOR>();

        pattern->Calculate(patternBuffer.as<T_COLOR>(), length, true, &params);

        fpsCounter.increaseUsedFrameCount();

        return patternBuffer;
    }

private:
    int length = 0;
    T_COLOR *ledData;
    Pattern<T_COLOR> *pattern;
};