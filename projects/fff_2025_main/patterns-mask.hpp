#pragma once
#include "hyperion.hpp"
#include <math.h>
#include <vector>
#include "core/generation/patterns/helpers/tempo/tempoListener.h"
#include <algorithm> // std::count

float fromTop3(float y){

    return Utils::rescale(y,0,1,-0.3,.5);
}


namespace CageMaskPatterns
{    
class GradientMaskPattern : public Pattern<RGBA>
    {

        Transition transition;
        PixelMap3d *map;
        bool direction = false; 

    public:
        GradientMaskPattern(PixelMap3d *map, bool direction = false)
        {
            this->name = direction ? "Gradient mask bottom" : "Gradient mask top";
            this->direction = direction;
            this->map = map;
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            for (int i = 0; i < width; i++)
            {
                float value = fromTop3(map->y(i));
                if (direction) value = 1. - value;
                pixels[i] = RGBA(0, 0, 0, 255) * value * transition.getValue();
            }
        }
    };
}