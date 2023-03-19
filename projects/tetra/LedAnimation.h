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

        Palette *colorPalette,

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

    template <class T, class A, class B, class C, class D>
    long map(T _x, A _in_min, B _in_max, C _out_min, D _out_max, typename std::enable_if<std::is_integral<T>::value>::type * = 0)
    {
        long x = _x, in_min = _in_min, in_max = _in_max, out_min = _out_min, out_max = _out_max;
        
        //return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;

        long in_range = in_max - in_min;
        long out_range = out_max - out_min;
        if (in_range == 0)
            return out_min + out_range / 2;
        long num = (x - in_min) * out_range;
        if (out_range >= 0)
        {
            num += in_range / 2;
        }
        else
        {
            num -= in_range / 2;
        }
        long result = num / in_range + out_min;
        if (out_range >= 0)
        {
            if (in_range * num < 0)
                return result - 1;
        }
        else
        {
            if (in_range * num >= 0)
                return result + 1;
        }
        return result;
        // more conversation:
        // https://forum.pjrc.com/threads/44503-map()-function-improvements
    }

    // // when the input is a float or double, do all math using the input's type
    // template <class T, class A, class B, class C, class D>
    // T map(T x, A in_min, B in_max, C out_min, D out_max, typename std::enable_if<std::is_floating_point<T>::value>::type * = 0)
    // {
    //     return (x - (T)in_min) * ((T)out_max - (T)out_min) / ((T)in_max - (T)in_min) + (T)out_min;
    // }
};
