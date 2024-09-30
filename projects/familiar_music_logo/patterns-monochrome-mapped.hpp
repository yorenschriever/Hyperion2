#pragma once
#include "core/generation/patterns/helpers/fade.h"
#include "core/generation/patterns/helpers/interval.h"
#include "core/generation/patterns/helpers/timeline.h"
#include "core/generation/patterns/pattern.hpp"
#include "core/generation/pixelMap.hpp"
#include "platform/includes/utils.hpp"
#include "utils.hpp"
#include <math.h>
#include <vector>

namespace MonochromeMappedPatterns
{

class SinPattern : public Pattern<Monochrome>
{
    LFO<NegativeCosFast> lfo;
    Transition transition = Transition(
        1000, Transition::none, 0,
        1000, Transition::none, 0);
    PixelMap *map;

public:
    SinPattern(PixelMap *map)
    {
        this->name = "Sin";
        this->map = map;
    }

    inline void Calculate(Monochrome *pixels, int width, bool active, Params *params) override
    {
        if (!transition.Calculate(active))
            return; // the fade out is done. we can skip calculating pattern data

        lfo.setPeriod(params->getVelocity(6000, 500));
        lfo.setDutyCycle(params->getSize());
        float amount = params->getAmount(0.25, 2.99);

        for (int index = 0; index < width; index++){
            float phase = (map->x(index) + map->y(index) +2)/4;
            pixels[index] = Monochrome(255 * lfo.getValue(amount * phase)) * transition.getValue(index, width);
        }
    }
};

class RadialFadePattern : public Pattern<Monochrome>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap::Polar *map;
        FadeDown fade = FadeDown(200);
        BeatWatcher watcher = BeatWatcher();

    public:
        RadialFadePattern(PixelMap::Polar *map)
        {
            this->map = map;
            this->name = "Radial fade";
        }

        inline void Calculate(Monochrome *pixels, int width, bool active, Params *params) override
        {
            fade.duration = params->getSize(500, 120);
            int velocity = params->getVelocity(1000, 50);

            if (!transition.Calculate(active))
                return;

            if (watcher.Triggered())
            {
                fade.reset();
            }

            for (int i = 0; i < map->size(); i++)
            {
                float conePos = map->r(i) / 2;
                pixels[i] += 255 * fade.getValue(conePos* velocity) * transition.getValue();
            }
        }
    };
}