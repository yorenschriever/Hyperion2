#pragma once
#include "core/generation/patterns/pattern.hpp"
#include "generation/patterns/helpers/fade.h"
#include "generation/patterns/helpers/interval.h"
#include "generation/patterns/helpers/timeline.h"
#include "generation/pixelMap.hpp"
#include <math.h>
#include <vector>
#include "mappingHelpers.hpp"
#include "log.hpp"


namespace Low
{

    class StaticGradientPattern : public Pattern<RGBA>
    {
        PixelMap3d *map;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        StaticGradientPattern(PixelMap3d *map)
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
                RGBA colour = params->gradient->get(h * 255);
                pixels[index] = colour * h * transition.getValue();
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
        PixelMap3d *map;
        Permute perm;
        int pos = 0;

    public:
        OnBeatColumnChaseUpPattern(PixelMap3d *map)
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
                    RGBA color = params->getPrimaryColour(); 
                    pixels[i] = color * fadePosition * (1 - y) * transition.getValue();
                }
            }
        }
    };

    class HorizontalSin : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<Glow> lfo;
        PixelMap3d::Cylindrical *map;

    public:
        HorizontalSin(PixelMap3d::Cylindrical *map)
        {
            this->map = map;
            this->name = "Horizontal sin";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(params->getVelocity(11000,500));
            lfo.setDutyCycle(params->getSize(0.03,0.5));

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                // RGBA color = params->getPrimaryColour(); 
                RGBA color = params->gradient->get(fromTop(map->z(index))*255); 
                pixels[index] = color * lfo.getValue(around(map->th(index))) * transition.getValue();
                // pixels[index] = color * lfo.getValue(fromTop(map->z(index))) * transition.getValue();
            }
        }

    };

    class HorizontalSaw : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<SawDown> lfo;
        PixelMap3d::Cylindrical *map;

    public:
        HorizontalSaw(PixelMap3d::Cylindrical *map)
        {
            this->map = map;
            this->name = "Horzontal saw";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(params->getVelocity(5000,500));
            lfo.setDutyCycle(params->getSize(0.06,1));
            bool orientationHorizontal = params->getVariant() > 0.5;

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                RGBA color = params->getPrimaryColour(); 
                float lfoArg = orientationHorizontal ? around(map->th(index)) : fromTop(map->z(index));
                pixels[index] = color * lfo.getValue(lfoArg) * fromBottom(map->z(index)) * transition.getValue();
            }
        }
    };

    class GrowShrink : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<SinFast> lfo;
        PixelMap3d::Cylindrical *map;

    public:
        GrowShrink(PixelMap3d::Cylindrical *map)
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
                //RGBA color = params->getPrimaryColour(); 
                //float lfoArg = orientationHorizontal ? around(map->th(index)) : fromTop(map->z(index));
                
                float lfoSize = lfo.getValue(offset * around(map->th(index))) * size; 
                float distance = abs(map->z(index) + 0.07);
                if (distance > lfoSize)
                    continue;

                float distanceAsRatio = 1 - distance / lfoSize ;

                pixels[index] = params->gradient->get(distanceAsRatio * 255) * distanceAsRatio * transition.getValue();
            }
        }
    };

    class GlowPulsePattern : public Pattern<RGBA>
    {
        Permute perm;
        LFO<Glow> lfo = LFO<Glow>(10000);
        Transition transition;
        PixelMap3d *map;

    public:
        GlowPulsePattern(PixelMap3d *map)
        {
            this->map = map;
            this->name = "Glow pulse";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            int density = width/481;
            lfo.setPeriod(params->getVelocity(10000,500));
            lfo.setDutyCycle(params->getAmount(0.05,0.5));
            float fadeSize = params->getSize(0,0.75)-0.5;
            perm.setSize(width);

            for (int index = 0; index < width; index++)
            {
                if (index % density != 0)
                   continue;
                float fade = Utils::constrain_f(fromBottom(map->y(perm.at[index]) - fadeSize),0,1);
                pixels[perm.at[index]] = params->getPrimaryColour() * fade * lfo.getValue(float(index)/width) * transition.getValue(index, width);
            }
        }
    };

 class VerticallyIsolated : public Pattern<RGBA>
    {
        Transition transition = Transition(
                200, Transition::fromLeft, 1200,
                500, Transition::fromRight, 1500);
        PixelMap3d::Cylindrical *map;

    public:
        VerticallyIsolated(PixelMap3d::Cylindrical *map)
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
                pixels[index] = params->gradient->get(z255)* transition.getValue(z255,255);
            }
        }
    };

     class RotatingRingsPattern : public Pattern<RGBA>
    {
        Transition transition;
        PixelMap3d::Cylindrical *map;
        LFO<Sin> ring1;
        LFO<Sin> ring2;

    public:
        RotatingRingsPattern(PixelMap3d::Cylindrical *map)
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
                float z_norm = zoffset+2*fromTop(map->z(index));
                float offset = around(map->th(index)) * params->getOffset();

                pixels[index] = col1 * softEdge(abs(z_norm - ring1.getValue(offset)), size);
                pixels[index] += col2 * softEdge(abs(z_norm - ring2.getValue(offset)), size);
            }
        }
    };

}
