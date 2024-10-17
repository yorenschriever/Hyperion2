#pragma once

#include "hyperion.hpp"
#include "MotorPosition.hpp"
#include <math.h>
#include <vector>

namespace Window
{

    class SinPattern : public Pattern<MotorPosition>
    {
        LFO<NegativeCosFast> lfo;
        Transition transition = Transition(
            1000, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        SinPattern()
        {
            this->name = "Sin";
        }

        inline void Calculate(MotorPosition *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            lfo.setPeriod(params->getVelocity(6000, 500));
            lfo.setDutyCycle(params->getSize());
            int amount = params->getAmount(1, 3.99);

            for (int index = 0; index < width; index++)
                pixels[index] = MotorPosition(0xFFFF * lfo.getValue(amount * float(index) / width)) * transition.getValue(index, width);
        }
    };

}