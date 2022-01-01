#pragma once
// #include "core/generation/patterns/pattern.hpp"
// #include "generation/patterns/helpers/fade.h"
// #include "generation/patterns/helpers/interval.h"
// #include "generation/patterns/helpers/timeline.h"
// #include "generation/pixelMap.hpp"
// #include "ledsterShapes.hpp"
// #include "mappingHelpers.hpp"
// #include <math.h>
// #include <vector>

namespace Hi
{

    class XY : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap3d *map;
        LFO<Sin> lfoX = LFO<Sin>(2000);
        LFO<Sin> lfoZ = LFO<Sin>(2000);

    public:
        XY(PixelMap3d *map)
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

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                float x_norm = (map->x(index)+1)/2.5;
                float z_norm = (map->z(index)+1)/2.5;

                float dim = std::max(
                    softEdge(abs(x_norm - lfoX.getValue()), size),
                    softEdge(abs(z_norm - lfoZ.getValue()*1.1), size)
                );

                pixels[index] = col * dim;
            }
        }
    };

class DotBeatPattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap3d::Spherical *map;
        FadeDown fade = FadeDown(200);
        BeatWatcher watcher = BeatWatcher();

    public:
        DotBeatPattern(PixelMap3d::Spherical *map)
        {
            this->map = map;
            this->name = "Dot beat";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            fade.duration = params->getIntensity(500, 100);

            if (watcher.Triggered())
                fade.reset();

            float size = params->getSize();

            for (int i = 0; i < map->size(); i++)
            {
                float radius = fade.getValue() * size;
                float normalized_angle = Utils::rescale_c(map->th(i), 0,1, 0, 0.8*M_PI);
                if (normalized_angle > radius)
                    continue;

                RGBA color = params->getGradient(radius * 255);
                float dim = normalized_angle / radius;
                pixels[i] = color * dim * transition.getValue();   
            }
        }
    };


}