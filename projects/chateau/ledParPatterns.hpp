#pragma once
#include "core/generation/patterns/helpers/fade.h"
#include "core/generation/patterns/helpers/interval.h"
#include "core/generation/patterns/helpers/timeline.h"
#include "core/generation/patterns/pattern.hpp"
#include "core/generation/pixelMap.hpp"
#include "platform/includes/utils.hpp"
#include "utils.hpp"
#include <math.h>
#include <vector>

namespace Ledpar
{

    class OnPattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            400, Transition::none, 0,
            400, Transition::none, 0);

    public:
        OnPattern()
        {
            this->name = "On";
        }
        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            for (int index = 0; index < width; index++)
                pixels[index] = params->getPrimaryColour() * transition.getValue();
        }
    };

    class SinPattern : public Pattern<RGBA>
    {
        LFO<SinFast> lfo;
        Transition transition = Transition(
            1000, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        SinPattern()
        {
            this->name = "Sin";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            lfo.setPeriod(params->getVelocity(6000, 500));
            lfo.setDutyCycle(params->getSize());
            
            for (int index = 0; index < width; index++)
                pixels[index] = params->getPrimaryColour() * lfo.getValue(float(index) / width) * transition.getValue(index, width);
        }
    };

    class FastStrobePattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            1000, Transition::none, 0,
            0, Transition::none, 0);

    public:
        FastStrobePattern()
        {
            this->name = "Fast strobe";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            RGBA value = Utils::millis() % 50 < 25 ? params->getHighlightColour() * transition.getValue() : RGBA();

            for (int index = 0; index < width; index++)
                pixels[index] = value;
        }
    };

    class SlowStrobePattern : public Pattern<RGBA>
    {
    public:
        SlowStrobePattern()
        {
            this->name = "Slow strobe";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            RGBA value = Utils::millis() % 100 < 25 ? params->getHighlightColour() : RGBA();

            for (int index = 0; index < width; index++)
                pixels[index] = value;
        }
    };

    class BlinderPattern : public Pattern<RGBA>
    {
    public:
        BlinderPattern(FadeShape in = Transition::none, FadeShape out = Transition::none, int fadein = 200, int fadeout = 600)
        {
            this->name = "Blinder";
        }

    protected:
        Transition transition;

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            for (int index = 0; index < width; index++)
                pixels[index] = params->getSecondaryColour() * transition.getValue();
        }
    };

    template <class T>
    class LfoPattern : public Pattern<RGBA>
    {
        LFO<T> lfo;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        LfoPattern()
        {
            this->name = "LFO";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(params->getVelocity(6000, 500));
            lfo.setDutyCycle(params->getSize());
            int amount = params->getAmount(1, 3.99);

            for (int index = 0; index < width; index++)
            {
                pixels[index] = params->getPrimaryColour() * transition.getValue() * lfo.getValue((float)amount * index / width);
            }
        }
    };

    class DuoTonePattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            400, Transition::none, 0,
            400, Transition::none, 0);

    public:
        DuoTonePattern()
        {
            this->name = "Duotone";
        }
        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            for (int index = 0; index < width; index++)
                pixels[index] = (index % 2 == 0 ? params->getPrimaryColour() : params->getSecondaryColour()) * transition.getValue();
        }
    };

    template <class T>
    class PaletteLfoPattern : public Pattern<RGBA>
    {
        LFO<T> lfo;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        PaletteLfoPattern()
        {
            this->name = "LFO Palette";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(params->getVelocity(6000, 500));
            lfo.setDutyCycle(params->getSize());
            int amount = params->getAmount(1, 3.99);

            for (int index = 0; index < width; index++)
            {
                float lfoVal = lfo.getValue((float)amount * index / width);
                pixels[index] = params->getGradient(255 * lfoVal) * transition.getValue();
            }
        }
    };

}