#pragma once
#include "core/generation/patterns/pattern.hpp"
#include "generation/patterns/helpers/fade.h"
#include "generation/patterns/helpers/interval.h"
#include "generation/patterns/helpers/timeline.h"
#include "generation/pixelMap.hpp"
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
        LFO<LFOPause<SawDown>> lfo = LFO<LFOPause<SawDown>>(2000);

    public:
        public:
        SnakePattern(){
            this->name = "Snake";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            lfo.setPeriod(params->getVelocity(10000, 500));
            lfo.setSkew(params->getIntensity());
            lfo.setPulseWidth(params->getSize(0.33, 1));
            int amount = params->getAmount() * 7 + 1;

            if (!transition.Calculate(active))
                return;

            for (int i = 0; i < LedsterShapes::snake.size(); i++)
            {
                float lfoVal = lfo.getValue((float)i / LedsterShapes::snake.size() * amount);
                pixels[LedsterShapes::snake[i]] += params->gradient->get(255 * lfoVal) * lfoVal * transition.getValue();
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

            lfo.setPeriod(params->getVelocity(4000,400));
            float phase = params->getOffset(0,45);

            //auto col = params->getPrimaryColour() * transition.getValue();
            int petalIndex=0;
            for (auto petal : LedsterShapes::petals)
            {
                for (int j = 0; j < 45; j++)
                {
                    //auto col = params->gradient->get(255 * j / 45);
                    float lfoVal = lfo.getValue((((float)j) + phase * petalIndex/6)/45.);
                    auto col = params->gradient->get(255 * lfoVal);
                    pixels[petal[j]] += col * lfoVal * transition.getValue();
                }
                petalIndex++;
            }
        }
    };

    class HexBeatPattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        FadeDown fade = FadeDown(200);
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

class DotBeatPattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap3d::Cylindrical map;
        FadeDown fade = FadeDown(200);
        BeatWatcher watcher = BeatWatcher();
        //Permute perm;

    public:
        DotBeatPattern(PixelMap3d::Cylindrical map)
        {
            this->map = map;
            //this->perm = Permute(map.size());
            this->name = "Dot beat";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            fade.duration = params->getIntensity(500, 100);

            if (watcher.Triggered())
                fade.reset();

            for (int i = 0; i < map.size(); i++)
            {
                if (map[i].z<0.44)
                    continue;;

                float radius = fade.getValue()*1.2;
                if (map[i].r > radius)
                    continue;

                RGBA color = params->gradient->get(radius * 255);
                float dim = map[i].r / radius;
                pixels[i] = color * dim * transition.getValue();   
            }
        }
    };


}