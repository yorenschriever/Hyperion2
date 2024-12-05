#pragma once
#include "hyperion.hpp"
#include <math.h>
#include <vector>

class GlowPulsePattern : public Pattern<RGB>
    {
        Permute perm;
        LFO<SinFast> lfo = LFO<SinFast>(10000);
        Transition transition;

    public:
        GlowPulsePattern(){
            this->name = "Glow pulse";
        }
        
        inline void Calculate(RGB *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            lfo.setPeriod(500 + 10000* (1.0f - params->getVelocity()));
            perm.setSize(width);

            for (int index = 0; index < width; index++)
            {
             float val = 1.025 * lfo.getValue(float(Transition::fromCenter(index,width,1000))/-1000);
            if (val < 1.0)
                pixels[perm.at[index]] = params->getPrimaryColour() * lfo.getValue(float(Transition::fromCenter(index,width,1000))/-1000);
            else
                pixels[perm.at[index]] = params->getPrimaryColour() + (params->getSecondaryColour() * (val-1)/0.025);            }
        }
    };