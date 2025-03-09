#pragma once
#include "hyperion.hpp"
#include <math.h>
#include <vector>

namespace SchwungMasks
{
    class OnBeatHexagonFade : public Pattern<RGBA>
    {
        int hex = 0;
        int hex2 = 0;
        int hex3 = 0;
        Transition transition;
        BeatWatcher watcher;
        FadeUp fade = FadeUp(2400, WaitAtStart);

    public:
        OnBeatHexagonFade()
        {
            this->name = "Onbeat Hex Fade";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            fade.setDuration(params->getVelocity(1000, 100));

            if (watcher.Triggered())
            {
                hex = (hex + Utils::random(0, 11)) % 12;
                do
                {
                    hex2 = (hex2 + Utils::random(0, 11)) % 12;
                } while (hex2 == hex);
                do
                {
                    hex3 = (hex3 + Utils::random(0, 11)) % 12;
                } while (hex3 == hex || hex3 == hex2);
                fade.reset();
            }

            for (int i = 0; i < width; i++)
            {
                pixels[i] = RGBA(0, 0, 0, 255) * transition.getValue();
            }

            for (int i = 0; i < 6 * 60; i++)
            {
                pixels[hexagons[hex][i]] = RGBA(0, 0, 0, 255) * fade.getValue() * transition.getValue();
            }

            if (params->getAmount() < 0.3)
                return;

            for (int i = 0; i < 6 * 60; i++)
            {
                pixels[hexagons[hex2][i]] = RGBA(0, 0, 0, 255) * fade.getValue() * transition.getValue();
            }

            if (params->getAmount() < 0.7)
                return;

            for (int i = 0; i < 6 * 60; i++)
            {
                pixels[hexagons[hex3][i]] = RGBA(0, 0, 0, 255) * fade.getValue() * transition.getValue();
            }
        }
    };

class ParallelMaskPattern : public Pattern<RGBA>
{
    Transition transition;

public:
ParallelMaskPattern()
    {
        this->name = "Parallel";
    }

    inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    {
        if (!transition.Calculate(active))
            return;

        for (int i = 0; i < width; i++)
        {
            pixels[i] = RGBA(0, 0, 0, 255) * transition.getValue();
        }

        for (auto hex : hexagons)
        {
            int offset = (int(params->getOffset(0,2.99)+2)%3)*60;
            for (int i = 0; i < 60; i++)
            {
                pixels[hex[(i + offset) % (6*60)]] = RGBA(0, 0, 0, 0) * transition.getValue();
                pixels[hex[(i + offset + 180) % (6*60)]] = RGBA(0, 0, 0, 0) * transition.getValue();
            }
        }


    }
};



class CentersMaskPattern : public Pattern<RGBA>
{
    Transition transition;

public:
CentersMaskPattern()
    {
        this->name = "Centers";
    }

    inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    {
        if (!transition.Calculate(active))
            return;

        for (int i = 0; i < width; i++)
        {
            int posInBar = i % 60;
            float dim = abs(posInBar - 30) / 30.0;

            pixels[i] = RGBA(0, 0, 0, 255) * dim * transition.getValue();
        }

    }
};

class EdgesMaskPattern : public Pattern<RGBA>
{
    Transition transition;

public:
EdgesMaskPattern()
    {
        this->name = "Edges";
    }

    inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    {
        if (!transition.Calculate(active))
            return;

        for (int i = 0; i < width; i++)
        {
            int posInBar = i % 60;
            float dim = 1. - abs(posInBar - 30) / 30.0;

            pixels[i] = RGBA(0, 0, 0, 255) * dim * transition.getValue();
        }

    }
};

class StarsMaskPattern : public Pattern<RGBA>
{
    Transition transition;

public:
StarsMaskPattern()
    {
        this->name = "Stars";
    }

    inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    {
        if (!transition.Calculate(active))
            return;

        for (int i = 0; i < width; i++)
        {
            int posInBar = i % 60;
            if (posInBar > 15 && posInBar < 45)
                pixels[i] = RGBA(0, 0, 0, 255) * transition.getValue();
            //else
            //  pixels[i] = RGBA(0, 0, 0, 255) * transition.getValue();
        }

    }
};

class XY : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap *map;
        LFO<Sin> lfoX = LFO<Sin>(2000);
        LFO<Sin> lfoZ = LFO<Sin>(2000);

    public:
        XY(PixelMap *map)
        {
            this->map = map;
            this->name="XY";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;
 
            auto col = RGBA(0,0,0,255);
            float size = params->getSize(0.01,0.03);
            lfoX.setPeriod(params->getVelocity(4*20000,2000));
            lfoZ.setPeriod(params->getVelocity(4*14000,1400));

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                float x_norm = (map->x(index)+1)/2;
                float z_norm = (map->y(index)+1)/2;

                float dim = 1.-std::max(
                    softEdge(abs(x_norm - lfoX.getValue()), size),
                    softEdge(abs(z_norm - lfoZ.getValue()*1.1), size)
                );

                pixels[index] = col * dim * transition.getValue();
            }
        }
    };

}