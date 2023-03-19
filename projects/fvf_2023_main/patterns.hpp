#pragma once
#include "core/generation/patterns/pattern.hpp"
#include "generation/pixelMap.hpp"
#include "generation/patterns/helpers/fade.h"
#include "generation/patterns/helpers/interval.h"
#include "generation/patterns/helpers/timeline.h"
#include <vector>
#include <math.h>

namespace FWF
{

    class RadialGlitterFadePattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PolarPixelMap map;
        FadeDown fade = FadeDown(200, WaitAtEnd);
        BeatWatcher watcher = BeatWatcher();
        Permute perm;
        float density;

    public:
        RadialGlitterFadePattern(PolarPixelMap map, float density=1)
        {
            this->map = map;
            this->perm = Permute(map.size());
            this->density = density;
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            fade.duration = Params::getIntensity(500,100);

            //timeline.FrameStart();
            //if (timeline.Happened(0))
            if (watcher.Triggered())
            {
                fade.reset();
                perm.permute();
            }

            float velocity = Params::getVelocity(600,100);
            //float trail = Params::getIntensity(0,1) * density;
            float trail = Params::getIntensity(0,200);

            for (int i = 0; i < map.size(); i++)
            {
                //fade.duration = perm.at[i] * trail; // + 100;
                //fade.duration = (perm.at[i] * trail) < map.size() / 20 ? ; // + 100;

                // fade.duration = 100;
                // if (perm.at[i] < density * map.size()/ 10)
                //     fade.duration *= 4;

                fade.duration = 100; //trail + perm.at[i] / (density * map.size()/ 10);
                if (perm.at[i] < density * map.size()/ 10)
                    fade.duration *= perm.at[i] * 4 / (density * map.size()/ 10);

                float fadePosition = fade.getValue(map[i].r * velocity);
                RGBA color = Params::palette->get(fadePosition * 255);
                pixels[i] = color * fadePosition * (1.5-map[i].r);
            }
        }
    };


    class PaletteGradientPattern : public Pattern<RGBA>
    {
        PolarPixelMap map;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        PaletteGradientPattern(PolarPixelMap map)
        {
            this->map = map;
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            for (int index = 0; index < std::min(width, (int)map.size()); index++)
            {
                RGBA colour = Params::palette->get(255 - map[index].r * 255);
                pixels[index] = colour * transition.getValue();
            }
        }
    };
};