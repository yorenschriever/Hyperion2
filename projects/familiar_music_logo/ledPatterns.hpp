
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

namespace LedPatterns {
    class OnPattern : public Pattern<RGBA>
    {
        RGB color;

        Transition transition = Transition(
            400, Transition::none, 0,
            400, Transition::none, 0);

    public:
        OnPattern(RGB color, const char* name )
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
}