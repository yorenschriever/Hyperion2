#pragma once
#include "hyperion.hpp"
#include <math.h>
#include <vector>

namespace MonochromePatterns
{

    class SinPattern : public Pattern<Monochrome>
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

        inline void Calculate(Monochrome *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            lfo.setPeriod(params->getVelocity(6000, 500));
            lfo.setDutyCycle(params->getSize());
            int amount = params->getAmount(1, 3.99);

            for (int index = 0; index < width; index++)
                pixels[index] = Monochrome(255 * lfo.getValue(amount * float(index) / width)) * transition.getValue(index, width);
        }
    };

    class GlowPattern : public Pattern<Monochrome>
    {
        Permute perm;
        LFO<Glow> lfo;
        Transition transition = Transition(
            1000, Transition::none, 0,
            2000, Transition::none, 0);

    public:
        GlowPattern(int direction = 1)
        {
            this->name = "Glow";
        }

        inline void Calculate(Monochrome *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            perm.setSize(width);

            float velocity = params->getVelocity(6000, 500);
            float amount = params->getAmount();
            lfo.setDutyCycle(amount);
            lfo.setPeriod(velocity / amount);

            for (int index = 0; index < width; index++)
                pixels[perm.at[index]] = Monochrome(255 * lfo.getValue(float(index) / width)) * transition.getValue(index, width);
        }
    };

}

namespace LedPatterns
{
    class OnPattern : public Pattern<RGBA>
    {
        RGB color;

        Transition transition = Transition(
            400, Transition::none, 0,
            400, Transition::none, 0);

    public:
        OnPattern(RGB color, const char *name)
        {
            this->color = color;
            this->name = name;
        }
        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            RGBA value = RGBA(color) * transition.getValue();

            for (int index = 0; index < width; index++)
                pixels[index] = value;
        }
    };

    class PalettePattern : public Pattern<RGBA>
    {
        int colorIndex;

        Transition transition = Transition(
            400, Transition::none, 0,
            400, Transition::none, 0);

    public:
        PalettePattern(int colorIndex, const char *name)
        {
            this->colorIndex = colorIndex;
            this->name = name;
        }
        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            RGBA col;
            if (colorIndex == 0)
                col = params->getPrimaryColour();
            if (colorIndex == 1)
                col = params->getSecondaryColour();

            RGBA value = col * transition.getValue();

            for (int index = 0; index < width; index++)
                pixels[index] = value;
        }
    };

    class GlowPattern : public Pattern<RGBA>
    {
        Permute perm;
        LFO<Glow> lfo = LFO<Glow>(10000);
        Transition transition;

    public:
        GlowPattern()
        {
            this->name = "Glow";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            float amount = params->getAmount();
            float velocity = params->getVelocity(10000, 500);

            lfo.setPeriod(velocity / amount);
            lfo.setDutyCycle(amount);
            perm.setSize(width);

            for (int index = 0; index < width; index++)
            {
                pixels[perm.at[index]] = RGBA(255,255,255,255) * lfo.getValue(float(index) / width) * transition.getValue(index, width);
            }
        }
    };

}