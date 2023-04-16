#pragma once
#include "core/generation/patterns/pattern.hpp"
#include "generation/patterns/helpers/fade.h"
#include "generation/patterns/helpers/interval.h"
#include "generation/patterns/helpers/timeline.h"
#include "generation/pixelMap.hpp"
#include "ledsterPatterns.hpp"
#include "ledsterShapes.hpp"
#include "mappingHelpers.hpp"
#include <math.h>
#include <vector>

namespace Hi
{

    class SnakePattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        //LFO<LFOPause<SawDown>> lfo = LFO<LFOPause<SawDown>>(2000);
        LFO<SawDownShort> lfo = LFO<SawDownShort>(2000);

    public:
        SnakePattern(){
            this->name = "Snake";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            lfo.setPeriod(params->getVelocity(10000, 500));
            lfo.setSkew(params->getIntensity());
            lfo.setPulseWidth(params->getSize(0.33, 1));
            int amount = 1;// params->getAmount(1,8) ;

            // lfo.setSkew(0.5);
            // lfo.setPulseWidth(1);
            // int variant = 5;

            if (!transition.Calculate(active))
                return;

            for (int i = 0; i < LedsterShapes::snake.size(); i++)
            {
                auto color = params->getSecondaryColour();
                pixels[LedsterShapes::snake[i]] = color * lfo.getValue((float)i / LedsterShapes::snake.size() * amount) * transition.getValue();
            }
        }
    };


    class XY : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap3d map;
        LFO<Sin> lfoX = LFO<Sin>(2000);
        LFO<Sin> lfoZ = LFO<Sin>(2000);

    public:
        XY(PixelMap3d map)
        {
            this->map = map;
            this->name="XY";
        }

        inline float softEdge(float dist, float size)
        {
            float edgeSize = 0.03;
            if (dist > size) return 0;
            
            if (dist < size - edgeSize) return 1;
            
            return (size-dist) / edgeSize;
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            
            auto col = params->getSecondaryColour() * transition.getValue();
            float size = params->getSize(0.01,0.03);
            lfoX.setPeriod(params->getVelocity(4*20000,2000));
            lfoZ.setPeriod(params->getVelocity(4*14000,1400));

            for (int index = 0; index < std::min(width, (int)map.size()); index++)
            {
                if (map[index].y < 0.44)
                    continue;

                float x_norm = (map[index].x+1)/2;
                float z_norm = (map[index].z+1)/2;

                float dim = std::max(
                    softEdge(abs(x_norm - lfoX.getValue()), size),
                    softEdge(abs(z_norm - lfoZ.getValue()*1.1), size)
                );

                pixels[index] = col * dim;
            }
        }
    };

    class PetalRotatePattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<SawDown> lfo = LFO<SawDown>(600);

    public:
        PetalRotatePattern(){
            this->name = "Petal Rotate";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(params->getVelocity(1000,400));
            float phase = params->getVariant(45,45*2);

            auto col = params->getSecondaryColour() * transition.getValue();
            for (auto petal : LedsterShapes::petals)
            {
                for (int j = 0; j < 45; j++)
                {
                    pixels[petal[j]] += col * lfo.getValue((float)j / phase);
                }
            }
        }
    };

    class HexBeatPattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        FadeDown fade = FadeDown(200, WaitAtEnd);
        BeatWatcher watcher = BeatWatcher();

    public:
        HexBeatPattern(){
            this->name = "Hex beat";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return;

            if (watcher.Triggered()){
                fade.reset();
            }

            fade.duration = params->getIntensity(300,50);
            int dist = params->getVelocity(100,15);

            for (int hex = 0; hex < 10; hex++)
            {
                RGBA colour = params->getSecondaryColour() * fade.getValue(hex * dist) * transition.getValue();
                for (int i = 0; i < LedsterShapes::hexagons[hex].size(); i++)
                {
                    pixels[LedsterShapes::hexagons[hex][i]] = colour;
                }
            }
        }
    };

}