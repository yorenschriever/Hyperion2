
#pragma once
#include "hyperion.hpp"
#include <math.h>
#include <vector>

namespace TestPatterns
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


    class HexChaser : public Pattern<RGBA>
    {
        Transition transition = Transition(
            400, Transition::none, 0,
            400, Transition::none, 0);

        LFO<SawDown> lfo;
    public:
        HexChaser()
        {
            this->name = "HexChaser";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            lfo.setDutyCycle(0.1);
            lfo.setPeriod(5000);

            for(auto hex: hexagons)
            {
                for (int i = 0; i < 6*60; i++)
                {
                    int index = hex[i];
                    RGBA value = RGBA(255, 0, 0,255) * lfo.getValue(float(i)/(6*60)) * transition.getValue();
                    pixels[index] += value;
                }
            }

            // auto hex = hexagons[3];
            // for (int i = 0; i < 6 * 60; i++)
            // {
            //     int index = hex[i];
            //     RGBA value = RGBA(255, 0, 0, 255) * lfo.getValue(float(i)/(6*60)) * transition.getValue();
            //     pixels[index] = value;
            // }
        }
    };
}