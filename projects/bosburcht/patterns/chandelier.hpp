#pragma once
#include "../mapping/ceilingMappedIndices.hpp"
#include "core/generation/patterns/pattern.hpp"
#include "generation/patterns/helpers/fade.h"
#include "generation/patterns/helpers/interval.h"
#include "generation/patterns/helpers/timeline.h"
#include "generation/pixelMap.hpp"
#include "mappingHelpers.hpp"
#include <math.h>
#include <vector>

namespace Patterns
{
    class StaticGradientPattern : public Pattern<RGBA>
    {
        PixelMap3d::Cylindrical *map;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        StaticGradientPattern(PixelMap3d::Cylindrical *map)
        {
            this->map = map;
            this->name = "Static gradient";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            float height = params->getSize(0.2, 0.75);

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                float h = map->r(index) * height;
                RGBA colour = params->getGradient(h * 255);
                pixels[index] = colour * h * transition.getValue();
            }
        }
    };

    class BreathingGradientPattern : public Pattern<RGBA>
    {
        PixelMap3d::Cylindrical *map;
        LFO<Glow> lfo;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        BreathingGradientPattern(PixelMap3d::Cylindrical *map)
        {
            this->map = map;
            this->name = "Breathing gradient";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            // float height = params->getSize(0.2,0.75);
            lfo.setPeriod(params->getVelocity(15000, 2000));
            float height = 0.25 + lfo.getValue() * params->getSize(0, 0.50);

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                float h = map->r(index) * height;
                RGBA colour = params->getGradient(h * 255);
                pixels[index] = colour * h * transition.getValue();
            }
        }
    };

    class BarLFO : public Pattern<RGBA>
    {
        int segmentSize;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<Glow> lfo;

    public:
        BarLFO(int segmentSize = 60)
        {
            this->segmentSize = segmentSize;
            this->name = "Bar LFO";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            int amount = 2*int(params->getAmount(1,5));
            lfo.setDutyCycle(params->getSize(0.25,1));
            lfo.setPeriod(params->getVelocity(1000,50)*amount);
            lfo.setSoftEdgeWidth(2*width/segmentSize);

            for (int bar = 0; bar < width; bar += segmentSize)
            {
                for (int i = 0; i < segmentSize; i++)
                    pixels[bar + i] = params->getSecondaryColour() * lfo.getValue(amount* float(bar) / width);
            }
        }
    };

    // class DotBeatPattern : public Pattern<RGBA>
    // {
    //     Transition transition = Transition(
    //         200, Transition::none, 0,
    //         1000, Transition::none, 0);
    //     PixelMap3d::Cylindrical *map;
    //     FadeDown fade = FadeDown(200);
    //     BeatWatcher watcher = BeatWatcher();

    // public:
    //     DotBeatPattern(PixelMap3d::Cylindrical *map)
    //     {
    //         this->map = map;
    //         this->name = "Dot beat";
    //     }

    //     inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    //     {
    //         if (!transition.Calculate(active))
    //             return;

    //         fade.duration = params->getIntensity(500, 100);

    //         if (watcher.Triggered())
    //             fade.reset();

    //         for (int i = 0; i < map->size(); i++)
    //         {

    //             float radius = fade.getValue()*1.2;
    //             if (map->r(i) > radius)
    //                 continue;

    //             RGBA color = params->getGradient(radius * 255);
    //             float dim = map->r(i) / radius;
    //             pixels[i] = color * dim * transition.getValue();
    //         }
    //     }
    // };


class GradientLFO : public Pattern<RGBA>
    {
        int segmentSize;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        //static const int numFades = 3;
        //int currentFade = 0;
        //FadeDown fade[numFades];
        BeatWatcher wachter;
        //int beatDivs[5] = {16, 8, 4, 2, 2};

        LFO<SawDown> lfo;

    public:
        GradientLFO(int segmentSize = 60)
        {
            this->segmentSize = segmentSize;
            this->name = "Gradient LFO";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            float trailSize = params->getAmount(30, 120);
            lfo.setPeriod(params->getVelocity(4000,500));
            lfo.setDutyCycle(params->getSize());

            for (int bar = 0; bar < width / segmentSize; bar++)
            {
                for (int i = 0; i < segmentSize; i++)
                {
                    int center = segmentSize / 2;
                    int distance = abs(center - i);

                    float fadeValue = lfo.getValue(float(bar) / (width/ segmentSize) * 2 + distance / trailSize);
                    pixels[bar * segmentSize + i] = params->getGradientf(fadeValue) * fadeValue * transition.getValue();
                }
            }
        }
    };


    class FadeFromRandomChandelier : public Pattern<RGBA>
    {
        int segmentSize;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        FadeDown fade;
        BeatWatcher watcher;
        int beatDivs[6] = {16, 8, 4, 2, 1, 1};
        int centers[48];
        int delays[48];
        FadeDown masterFade;

    public:
        FadeFromRandomChandelier(int segmentSize = 60)
        {
            this->segmentSize = segmentSize;
            this->name = "Fade from random";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            int beatDiv = beatDivs[int(params->getAmount(0, 5))];
            if (this->watcher.Triggered() && Tempo::GetBeatNumber() % beatDiv == 0)
            {
                fade.reset();
                masterFade.reset();
                for (int i = 0; i < 48; i++)
                {
                    centers[i] = Utils::random(15, 45);
                    delays[i] = Utils::random(0, 500);
                }
            }

            if (!transition.Calculate(active))
                return;

            masterFade.duration = Tempo::TimeBetweenBeats() * beatDiv;

            float trailSize = params->getVelocity(15, 5);
            fade.setDuration(params->getSize(1200, 300));
            float offset = params->getOffset();

            for (int bar = 0; bar < width / segmentSize; bar++)
            {
                for (int i = 0; i < segmentSize; i++)
                {
                    int distance = abs(centers[bar] - i);
                    float fadeValue = fade.getValue(distance * trailSize + delays[bar] * offset);
                    pixels[bar * segmentSize + i] += params->getPrimaryColour() * fadeValue * masterFade.getValue() * transition.getValue();
                }
            }
        }
    };

}