#pragma once
#include "core/generation/patterns/pattern.hpp"
#include "generation/patterns/helpers/fade.h"
#include "generation/patterns/helpers/interval.h"
#include "generation/patterns/helpers/timeline.h"
#include "generation/pixelMap.hpp"
#include <math.h>
#include <vector>

namespace Halo
{

    class PrimaryColorPattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            500, Transition::fromCenter, 500,
            500, Transition::fromCenter, 1000);

    public:
        PrimaryColorPattern()
        {
            this->name = "Primary color";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            for (int i = 0; i < width; i++)
            {
                pixels[i] = params->getPrimaryColour() * transition.getValue(i, width);
            }
        }
    };

    class SecondaryColorPattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            500, Transition::fromCenter, 500,
            500, Transition::fromCenter, 1000);

    public:
        SecondaryColorPattern()
        {
            this->name = "Secondary color";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            for (int i = 0; i < width; i++)
            {
                pixels[i] = params->getSecondaryColour() * transition.getValue(i, width);
            }
        }
    };

    class SinChasePattern : public Pattern<RGBA>
    {

        Transition transition;
        LFO<LFOPause<NegativeCosFast>> lfo;

    public:
        SinChasePattern()
        {
            this->name = "Sin chase";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPulseWidth(params->getSize(0.1,1));
            lfo.setPeriod(params->getVelocity(4000,500));
            int amount = params->getAmount(1,5);

            for (int i = 0; i < width; i++)
            {
                float phase = ((float)i/width) * amount;
                pixels[i] = params->getSecondaryColour() * lfo.getValue(phase) * transition.getValue();
            }
        }
    };

    class SawChasePattern : public Pattern<RGBA>
    {

        Transition transition;
        LFO<LFOPause<SawDown>> lfo;

    public:
        SawChasePattern()
        {
            this->name = "Saw chase";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPulseWidth(params->getSize(0.1,1));
            lfo.setPeriod(params->getVelocity(4000,500));
            int amount = params->getAmount(1,5);
            int variantParam = std::min((int)params->getVariant(1,3),2);

            for (int variant = 0; variant < variantParam; variant++)
            {
                for (int i = 0; i < width; i++)
                {
                    float phase = ((float)i/width) * amount;
                    if (variant == 1) phase  = 1.0 - phase;
                    float lfoVal =  lfo.getValue(phase);
                    pixels[i] += params->gradient->get(255*lfoVal) * lfoVal * transition.getValue();
                }
            }
        }
    };

    class StrobePattern : public Pattern<RGBA>
    {
        int framecounter = 1;

    public:
        StrobePattern()
        {
            this->name = "Strobe palette";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            framecounter--;

            RGBA color = RGBA(0, 0, 0, 255);
            if (framecounter <= 1)
                color = params->getPrimaryColour();

            if (framecounter == 0)
                framecounter = 5; // params->getVelocity(40,4);

            for (int index = 0; index < width; index++)
                pixels[index] = color;
        }
    };

    class SinAllPattern : public Pattern<RGBA>
    {

        Transition transition;
        LFO<Sin> lfo;

    public:
        SinAllPattern()
        {
            this->name = "All sin";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            //lfo.setPulseWidth(params->getSize(0.1,1));
            lfo.setPeriod(params->getVelocity(4000,100));
            //int amount = params->getAmount(1,5);

            for (int i = 0; i < width; i++)
            {
                pixels[i] = params->getSecondaryColour() * lfo.getValue() * transition.getValue();
            }
        }
    };

}