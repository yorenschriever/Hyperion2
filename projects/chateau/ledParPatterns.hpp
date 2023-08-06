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
        uint8_t intensity;

        Transition transition = Transition(
            400, Transition::none, 0,
            400, Transition::none, 0);

    public:
        OnPattern(uint8_t intensity = 100)
        {
            this->intensity = intensity;
            this->name = "On";
        }
        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            for (int index = 0; index < width; index++)
                pixels[index] = params->getPrimaryColour();
        }
    };

    class SinPattern : public Pattern<RGBA>
    {
        int direction;
        float phase;

        LFO<SinFast> lfo;
        Transition transition = Transition(
            1000, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        SinPattern(int direction = 4, float phase = 0)
        {
            this->direction = direction;
            this->phase = phase;
            this->name = "Sin";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            for (int index = 0; index < width; index++)
                pixels[index] = params->getPrimaryColour() * lfo.getValue(float(index) / width, direction * 250) * transition.getValue(index, width);
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

            RGBA value = Utils::millis() % 50 < 25 ? params->getHighlightColour() * transition.getValue() : RGBA() ;

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

            RGBA value = Utils::millis() % 100 < 25 ? params->getHighlightColour() : RGBA() ;

            for (int index = 0; index < width; index++)
                pixels[index] = value;
        }
    };

    class BlinderPattern : public Pattern<RGBA>
    {
    public:
        BlinderPattern(FadeShape in = Transition::none, FadeShape out = Transition::none, int fadein = 200, int fadeout = 600)
        {
            transition = Transition(
                fadein, in, 200,
                fadeout, out, 500);
            this->name = "Blinder";
        }

    protected:
        Transition transition;
        
        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            for (int index = 0; index < width; index++)
                pixels[index] = params->getSecondaryColour() * transition.getValue(index, width);
        }
    };

    template <class T>
    class LFOPattern : public Pattern<RGBA>
    {
        int numWaves;
        LFO<T> lfo;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        LFOPattern(int numWaves, int period = 5000, float pulsewidth = 0.5)
        {
            this->numWaves = numWaves;
            this->lfo = LFO<T>(period);
            this->lfo.setDutyCycle(pulsewidth);
            this->name = "LFO";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            for (int index = 0; index < width; index++)
            {
                pixels[index] = params->getPrimaryColour() * transition.getValue() * lfo.getValue((float)numWaves * index / width);
            }
        }
    };

}