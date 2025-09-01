#pragma once
#include "hyperion.hpp"
#include <math.h>
#include <vector>

namespace EffectPatterns
{
    class OnPattern : public Pattern<Effect>
    {
        uint8_t intensity;
        
    public:
        OnPattern(uint8_t intensity = 180, const char *name = "on")
        {
            this->intensity = intensity;
            this->name = name;
        }

        inline void Calculate(Effect *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            for (int index = 0; index < width; index++)
                pixels[index] = intensity;
        }
    };


    class IntervalPattern : public Pattern<Effect>
    {
        LFOTempo<PWM> lfo;
        
    public:
        IntervalPattern(const char *name = "on")
        {
            this->name = name;
        }

        inline void Calculate(Effect *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            lfo.setPeriodExponential((int)params->getVelocity(1,3.99));
            lfo.setDutyCycle(params->getAmount(0.0,1) );
            int intensity = (uint8_t)params->getSize(0,255);

            if (!lfo.getValue())
                return;

            for (int index = 0; index < width; index++)
                pixels[index] = intensity;
        }
    };

}