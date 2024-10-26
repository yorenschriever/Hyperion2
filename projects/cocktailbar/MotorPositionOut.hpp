#pragma once

#include "core/colours.h"

class MotorPositionOut : Colour
{
public:
    MotorPositionOut()
    {
    }

    MotorPositionOut(uint16_t position)
    {
        this->positionH = position >> 8;
        this->positionL = position & 0xFF;
    }

    inline void dim(uint8_t value)
    {
        //do nothing
    }

    inline void ApplyLut(LUT *lut)
    {
        //do nothing
    }

    uint8_t positionL = 0;
    uint8_t positionH = 0;
};