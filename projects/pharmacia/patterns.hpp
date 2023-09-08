#pragma once
#include "core/generation/patterns/pattern.hpp"
#include "generation/patterns/helpers/fade.h"
#include "generation/patterns/helpers/interval.h"
#include "generation/patterns/helpers/timeline.h"
#include "generation/pixelMap.hpp"
#include <math.h>
#include <vector>

namespace Patterns
{

    class Cross : public Pattern<RGBA>
    {
        static const int numCrosses = 6;
        static const int duration = 700;
        Transition transition;
        PixelMap *map;
        LFO<SawUp> lfo = LFO<SawUp>(duration);
        Timeline timeline = Timeline(duration);
        LFO<SawUp> lfoCenter = LFO<SawUp>(5000);
        float x_start[numCrosses];
        float y_start[numCrosses];

    public:
        Cross(PixelMap *map)
        {
            this->map = map;
            this->name = "Cross tunnel";
        }

        bool crossOutline(float x, float y, float sz, float w)
        {
            // every quadrant is a mirrored, only write logic for 1 quadrant (top right)
            x = std::abs(x);
            y = std::abs(y);
            // the quadrant is mirrored diagonally. take the top left
            if (y < x)
                std::swap(x, y);

            // the horizontal part
            if (x < (sz / 3) + w && std::abs(y - 2 * w - sz) <= w)
                return true;

            // the vertical part
            if (std::abs(x - sz / 3) < w && y > (sz / 3) - w && y - 2 * w < sz)
                return true;

            return false;
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            timeline.FrameStart();

            // determine the render order. Render the cross at the back first, so they are not on top
            // of the ones in the front.
            // tbh i started deriving this theoretically, but in the end fiddled with trial and error
            // until it worked.
            float lfoVal0 = lfo.getValue();
            int renderOrder[numCrosses];
            int back = (numCrosses - int(numCrosses * lfoVal0) - 1) % numCrosses;
            for (int i = 0; i < numCrosses; i++)
                renderOrder[numCrosses - 1 - (back + i) % numCrosses] = i;

            for (int cross_i = 0; cross_i < numCrosses; cross_i++)
            {
                int cross = renderOrder[cross_i];
                float lfoVal = lfo.getValue(float(cross) / numCrosses);

                // reset the center starting point at the same time an LFO will reset to zero (starts as small cross again)
                if (timeline.Happened(duration * float(cross) / numCrosses))
                {
                    x_start[cross] = 0.56 * cos(lfoCenter.getValue() * 2 * 3.1415);
                    y_start[cross] = 0.56 * sin(2 * lfoCenter.getValue() * 2 * 3.1415);
                }

                // render the cross
                for (int index = 0; index < std::min(width, (int)map->size()); index++)
                {
                    float x_c = x_start[cross] * (1.0 - lfoVal);
                    float y_c = y_start[cross] * (1.0 - lfoVal);
                    if (crossOutline(map->x(index) - x_c, map->y(index) - y_c, lfoVal, Utils::rescale(lfoVal, 0.01, 0.04)))
                        // pixels[index] = params->getPrimaryColour() * Utils::rescale(lfoVal, 0.1, 1);
                        pixels[index] = params->gradient->get(125 - 125 * lfoVal)* Utils::rescale(lfoVal, 0.5, 1);
                }
            }

            // render a static cross outline at the border
            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                if (crossOutline(map->x(index), map->y(index), 0.9, 0.02))
                    pixels[index] = params->getPrimaryColour();
            }
        }
    };

}