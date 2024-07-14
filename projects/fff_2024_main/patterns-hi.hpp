#pragma once
#include "mappingHelpers.hpp"
#include <math.h>
#include <vector>

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
                if (map->y(index) < 0.44)
                    continue;

                float x_norm = (map->x(index)+1)/2;
                float z_norm = (map->z(index)+1)/2;

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
        PixelMap3d::Cylindrical *map;
        FadeDown fade = FadeDown(200);
        BeatWatcher watcher = BeatWatcher();
        //Permute perm;

    public:
        DotBeatPattern(PixelMap3d::Cylindrical *map)
        {
            this->map = map;
            //this->perm = Permute(map->size());
            this->name = "Dot beat";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            fade.duration = params->getIntensity(500, 100);

            if (watcher.Triggered())
                fade.reset();

            for (int i = 0; i < map->size(); i++)
            {
                if (map->z(i)<0.44)
                    continue;;

                float radius = fade.getValue()*1.2;
                if (map->r(i) > radius)
                    continue;

                RGBA color = params->getGradient(radius * 255);
                float dim = map->r(i) / radius;
                pixels[i] = color * dim * transition.getValue();   
            }
        }
    };


}