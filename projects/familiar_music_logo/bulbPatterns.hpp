#pragma once
#include "core/generation/patterns/helpers/fade.h"
#include "core/generation/patterns/helpers/interval.h"
#include "core/generation/patterns/helpers/timeline.h"
#include "core/generation/patterns/pattern.hpp"
#include "core/generation/pixelMap.hpp"
#include "platform/includes/utils.hpp"
#include "colours.h"
#include <math.h>
#include <vector>

namespace BulbPatterns {
    class OnPattern : public Pattern<Monochrome>
    {
        uint8_t intensity;

        Transition transition = Transition(
            400, Transition::none, 0,
            400, Transition::none, 0);

    public:
        OnPattern(uint8_t intensity = 180)
        {
            this->intensity = intensity;
            this->name = "On";
        }
        inline void Calculate(Monochrome *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            Monochrome value = Monochrome(intensity) * transition.getValue();

            for (int index = 0; index < width; index++)
                pixels[index] = value;
        }
    };
}