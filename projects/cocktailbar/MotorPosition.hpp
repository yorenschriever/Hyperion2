#pragma once

#include "core/colours.h"

class MotorPosition : Colour
{
public:
    MotorPosition()
    {
        this->position=0;
    }

    MotorPosition(uint16_t position)
    {
        this->position = position;

    }

    inline void dim(uint8_t value)
    {
        //do nothing
    }

    inline void ApplyLut(LUT *lut)
    {
        //do nothing
    }

    MotorPosition operator+(MotorPosition other)
    {
        MotorPosition result = MotorPosition(*this);
        result += other;
        return result;
    }

    MotorPosition &operator+=(const MotorPosition &other)
    {
        // this = bottom
        // other = top

        position = other.position;

        return *this;
    }

    MotorPosition operator*(float scale)
    {
        return MotorPosition(position * scale);
    }

    uint16_t position = 0;
};