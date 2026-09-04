
#pragma once
#include "hyperion.hpp"
#include "pattern-helpers.hpp"
#include <math.h>
#include <vector>

namespace Mapped3dPatterns
{
    class StaticGradientPattern : public Pattern<RGBA>
    {
        PixelMap3dPtr map;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        StaticGradientPattern(PixelMap3dPtr map)
        {
            this->map = map;
            this->name = "Static gradient";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return;

            float height = params->getSize(0.2,1);

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                float h = fromBottom(map->y(index))* height;
                RGBA color = params->getGradient(h * 255);
                pixels[index] = color * h * transition.getValue();
            }
        }
    };

    class OnBeatColumnChaseUpPattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        FadeDown fade[6] = {
            FadeDown(200),
            FadeDown(200),
            FadeDown(200),
            FadeDown(200),
            FadeDown(200),
            FadeDown(200)};
        BeatWatcher watcher = BeatWatcher();
        PixelMap3dPtr map;
        Permute perm;
        int pos = 0;

    public:
        OnBeatColumnChaseUpPattern(PixelMap3dPtr map)
        {
            this->map = map;
            this->perm = Permute(map->size());
            this->name = "On beat column chase up";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return;

            if (watcher.Triggered())
            {
                pos = (pos + 1) % 6;
                fade[pos].reset();
            }

            float velocity = params->getVelocity(2000, 100);
            float tailSize = params->getSize(300,50);

            for (int column = 0; column < 6; column++)
            {
                int columnStart = column * width / 6;
                int columnEnd = columnStart + width / 6;

                fade[column].duration = tailSize;

                for (int i = columnStart; i < columnEnd; i++)
                {
                    float y = fromTop(map->y(i));
                    float fadePosition = fade[column].getValue(y * velocity);
                    RGBA color = params->getPrimaryColor(); 
                    pixels[i] = color * fadePosition * (1 - y) * transition.getValue();
                }
            }
        }
    };

    class GrowShrink : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<SinFast> lfo;
        PixelMap3d::CylindricalPtr map;

    public:
        GrowShrink(PixelMap3d::CylindricalPtr map)
        {
            this->map = map;
            this->name = "Grow shrink";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(params->getVelocity(5000,500));
            //lfo.setPulseWidth(params->getSize(0.06,1));
            float size = params->getSize(0.1,0.5);
            float offset = params->getOffset(0,1);

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                //RGBA color = params->getPrimaryColor(); 
                //float lfoArg = orientationHorizontal ? around(map->th(index)) : fromTop(map->z(index));
                
                float lfoSize = lfo.getValue(offset * around(map->th(index))) * size; 
                float distance = abs(map->z(index) + 0.07);
                if (distance > lfoSize)
                    continue;

                float distanceAsRatio = 1 - distance / lfoSize ;

                pixels[index] = params->getGradient(distanceAsRatio * 255) * distanceAsRatio * transition.getValue();
            }
        }
    };

    class VerticallyIsolated : public Pattern<RGBA>
    {
        Transition transition = Transition(
                200, Transition::fromStart, 1200,
                500, Transition::fromEnd, 1500);
        PixelMap3d::CylindricalPtr map;

    public:
        VerticallyIsolated(PixelMap3d::CylindricalPtr map)
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
                int angle = around(map->th(index)) * 3600;
                if ((angle+300) % 600 > 0)
                    continue;

                int z255 = fromBottom(map->z(index)) * 255;
                pixels[index] = params->getGradient(z255)* transition.getValue(z255,255);
            }
        }
    };

    class RotatingRingsPattern : public Pattern<RGBA>
    {
        Transition transition;
        PixelMap3d::CylindricalPtr map;
        LFO<Sin> ring1;
        LFO<Sin> ring2;

    public:
        RotatingRingsPattern(PixelMap3d::CylindricalPtr map)
        {
            this->map = map;
            this->name = "Rotating rings";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            auto col1 = params->getPrimaryColor() * transition.getValue();
            auto col2 = params->getSecondaryColor() * transition.getValue();
            float size = params->getSize(0.01,0.1);
            float zoffset = params->getVariant(0,-0.2);
            ring1.setPeriod(params->getVelocity(20000,2000));
            ring2.setPeriod(params->getVelocity(14000,1400));

            for (int index = 0; index < width; index++)
            {
                float z_norm = zoffset+2*fromTop(map->z(index));
                float offset = around(map->th(index)) * params->getOffset();

                pixels[index] = col1 * softEdge(abs(z_norm - ring1.getValue(offset)), size);
                pixels[index] += col2 * softEdge(abs(z_norm - ring2.getValue(offset)), size);
            }
        }
    };
}
