
#pragma once
#include "hyperion.hpp"
#include <math.h>
#include <vector>

namespace MaskPatterns
{
    class SinChaseMaskPattern : public Pattern<RGBA>
    {

        Transition transition;
        LFO<Glow> lfo;

    public:
        SinChaseMaskPattern()
        {
            this->name = "Sin chase Mask";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setDutyCycle(params->getSize(0.1, 1));
            lfo.setPeriod(params->getVelocity(4000, 500));
            int amount = params->getAmount(1, 3);
            float offset = params->getOffset(0, 5);

            for (int i = 0; i < width; i++)
            {
                float phase = ((float)i / width) * amount * 48 + float(i % (width / 2)) * offset / width;
                pixels[i] = RGBA(0, 0, 0, 255) * (1. - lfo.getValue(phase)) * transition.getValue();
            }
        }
    };

    class GlowPulseMaskPattern : public Pattern<RGBA>
    {
        Permute perm;
        LFO<Glow> lfo = LFO<Glow>(10000);
        Transition transition;

    public:
        GlowPulseMaskPattern()
        {
            this->name = "Glow pulse mask";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            float amount = params->getAmount(0.005, 0.1);
            lfo.setPeriod(params->getVelocity(3000, 500) / amount);
            lfo.setDutyCycle(amount);
            perm.setSize(width);

            for (int index = 0; index < width; index++)
            {
                pixels[perm.at[index]] = RGBA(0, 0, 0, 255) * (1. - lfo.getValue(float(index) / width)) * transition.getValue(index, width);
            }
        }
    };

    class SegmentGradientMaskPattern : public Pattern<RGBA>
    {

        Transition transition;
        LFO<Glow> lfo;
        int segmentSize;
        bool direction = false; 

    public:
        SegmentGradientMaskPattern(int segmentSize=60, bool direction = false)
        {
            this->name = "Gradient mask";
            this->segmentSize = segmentSize;
            this->direction = direction;
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            for (int i = 0; i < width; i++)
            {
                float value = (i % segmentSize) / float(segmentSize);
                if (direction) value = 1. - value;
                pixels[i] = RGBA(0, 0, 0, 255) * (1. - value) * transition.getValue();
            }
        }
    };
}