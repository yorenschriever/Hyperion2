#pragma once

#include "core/colours.h"
#include "MotorPositionOut.hpp"

class MotorPosition : Colour
{
public:
    MotorPosition()
    {
        this->position=0;
        this->alpha=0;
    }

    MotorPosition(uint16_t position, uint8_t alpha)
    {
        this->position = position;
        this->alpha = alpha;
    }
    
    operator MotorPositionOut() { 
        return MotorPositionOut((position * alpha / 255)); 
    }

    inline void dim(uint8_t value)
    {
        alpha = (alpha * value) / 255;
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

        if (other.alpha == 255)
        {
            alpha=255;
            position = other.position;
            return *this;
        }

        int outA = other.alpha * 255 + (this->alpha * (255 - other.alpha));

        if (outA == 0)
        {
            this->position = 0;
            this->alpha = 0;
            return *this;
        }

        this->position = ((other.position * other.alpha) * 255 + (this->position * this->alpha) * (255 - other.alpha)) / outA;
        this->alpha = outA / 255;

        //Log::info("","alpha %d",  this->alpha);

        if (alpha >= 254)
            alpha = 255;

        if (alpha <= 2)
            alpha = 0;

        return *this;
    }

    MotorPosition operator*(float scale)
    {
        return MotorPosition(position, scale * alpha);
    }

    uint16_t position = 0;
    uint8_t alpha = 0;
};

