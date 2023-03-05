#pragma once
// -------------------------------------------------------------------- Includes
#include <vector>
using namespace std;

#include "LedShape.h"
#include "Palette.h"
#include "colours.h"
#include <stdint.h>

// -------------------------------------------------------------------- Class Definition
class LedAnimation
{
public:
    virtual void render(
        RGBA *buffer,
        int buffersize,
        vector<LedShape> ledShapes,

        Palette colorPalette,

        uint8_t speed,
        uint8_t amount,
        uint8_t size,
        uint8_t direction,
        uint8_t offset,
        uint8_t alpha,

        uint16_t beatProgress,
        uint16_t beatProgressMax,
        uint16_t beatCount,
        bool updateCache) = 0;

protected:
    uint8_t lerp8by8(uint8_t a, uint8_t b, uint8_t frac)
    {
        uint8_t result;
        if (b > a)
        {
            uint8_t delta = b - a;
            uint8_t scaled = scale8(delta, frac);
            result = a + scaled;
        }
        else
        {
            uint8_t delta = a - b;
            uint8_t scaled = scale8(delta, frac);
            result = a - scaled;
        }
        return result;
    }

    uint16_t scale16by8(uint16_t i, uint8_t scale)
    {
        return (i * (1 + ((uint16_t)scale))) >> 8;
    }

    RGBA blend(RGBA col1, RGBA col2, uint8_t frac)
    {
        return col1 + (col2 * (float(frac) / 255.));
    }

    uint8_t scale8(uint8_t i, uint8_t scale)
    {
        return (((uint16_t)i) * (1 + (uint16_t)(scale))) >> 8;
    }
};
