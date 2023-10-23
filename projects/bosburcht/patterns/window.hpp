#pragma once
#include "core/generation/patterns/pattern.hpp"
#include "generation/patterns/helpers/fade.h"
#include "generation/patterns/helpers/interval.h"
#include "generation/patterns/helpers/timeline.h"
#include "generation/pixelMap.hpp"
#include "mappingHelpers.hpp"
#include <math.h>
#include <vector>
#include "../mapping/ceilingMappedIndices.hpp"

namespace Patterns
{

    class GrowShrink : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<NegativeCosFast> lfo;
        PixelMap3d *map;

    public:
        GrowShrink(PixelMap3d *map)
        {
            this->map = map;
            this->name = "Grow shrink";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(params->getVelocity(5000,500));
            lfo.setDutyCycle(params->getAmount(0.3,1));
            float size = params->getSize(0.1,0.4);
            float offset = params->getOffset(0,1);

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                float lfoSize = lfo.getValue(offset * (map->z(index)+1)/2) * size; 

                float distance = abs(map->y(index) );
                if (distance > lfoSize)
                    continue;

                float distanceAsRatio = 1 - distance / lfoSize ;

                pixels[index] = params->gradient->get(distanceAsRatio * 255) * distanceAsRatio * transition.getValue();
            }
        }
    };

class VerticallyIsolated : public Pattern<RGBA>
    {
        Transition transition = Transition(
                200, Transition::fromStart, 1200,
                500, Transition::fromEnd, 1500);
        PixelMap3d *map;

    public:
        VerticallyIsolated(PixelMap3d *map)
        {
            this->map = map;
            this->name = "Vertically isolated";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            for (int index = 0; index < width; index++)
            {
                int z255 = fromBottom(map->y(index)) * 255;
                pixels[index] = params->gradient->get(z255)* transition.getValue(255-z255,255);
            }
        }
    };

     class RotatingRingsPattern : public Pattern<RGBA>
    {
        Transition transition;
        PixelMap3d *map;
        LFO<Sin> ring1;
        LFO<Sin> ring2;

    public:
        RotatingRingsPattern(PixelMap3d *map)
        {
            this->map = map;
            this->name = "Rotating rings";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            auto col1 = params->getPrimaryColour() * transition.getValue();
            auto col2 = params->getSecondaryColour() * transition.getValue();
            float size = params->getSize(0.01,0.1);
            float zoffset = params->getVariant(0,-0.2);
            ring1.setPeriod(params->getVelocity(20000,2000));
            ring2.setPeriod(params->getVelocity(14000,1400));

            for (int index = 0; index < width; index++)
            {
                float z_norm = fromBottom(map->y(index));
                float offset = (map->z(index)+2)/2 * params->getOffset();

                pixels[index] = col1 * softEdge(abs(z_norm - ring1.getValue(offset)), size);
                pixels[index] += col2 * softEdge(abs(z_norm - ring2.getValue(offset)), size);
            }
        }
    };

    class OnBeatWindowChaseUpPattern : public Pattern<RGBA>
    {
        static const int numWindows = 16;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        FadeDown fade[numWindows] = {
            FadeDown(200),
            FadeDown(200),
            FadeDown(200),
            FadeDown(200),
            FadeDown(200),
            FadeDown(200),
            FadeDown(200),
            FadeDown(200),
            FadeDown(200),
            FadeDown(200),
            FadeDown(200),
            FadeDown(200),
            FadeDown(200),
            FadeDown(200),
            FadeDown(200),
            FadeDown(200),
            };
        BeatWatcher watcher = BeatWatcher();
        PixelMap3d *map;
        
        int pos = 0;

    public:
        OnBeatWindowChaseUpPattern(PixelMap3d *map)
        {
            this->map = map;
            //this->perm = Permute(map->size());
            this->name = "On beat window chase up";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return;

            if (watcher.Triggered())
            {
                pos = (pos + 1) % numWindows;
                fade[pos].reset();
            }

            float velocity = params->getVelocity(2000, 100);
            float tailSize = params->getSize(300,50);

            for (int column = 0; column < numWindows; column++)
            {
                int columnStart = column * width / numWindows;
                int columnEnd = columnStart + width / numWindows;

                fade[column].duration = tailSize;

                for (int i = columnStart; i < columnEnd; i++)
                {
                    float y = fromBottom(map->y(i));
                    float fadePosition = fade[column].getValue(y * velocity);
                    RGBA color = params->getPrimaryColour(); 
                    pixels[i] = color * fadePosition * (1 - y) * transition.getValue();
                }
            }
        }
    };
};