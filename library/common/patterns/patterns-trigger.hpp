#pragma once
#include "hyperion.hpp"
#include <math.h>
#include <vector>

namespace TriggerPatterns
{

    class FadingNoisePattern : public Pattern<RGBA>
    {
        Fade<Down, Cubic> fade = Fade<Down, Cubic>(600);
        Permute perm;
        bool lastActive;

    public:
        FadingNoisePattern()
        {
            this->name = "Fading noise";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (active && !lastActive)
                fade.reset();
            lastActive = active;

            if (fade.getPhase() == 1)
                return;

            perm.setSize(width);
            perm.permute();

            int noiseLevel = fade.getValue() * width / 3;
            for (int index = 0; index < noiseLevel; index++)
            {
                pixels[perm.at[index]] = params->getHighlightColor();
            }
        }
    };

    class PulsePattern : public Pattern<RGBA>
    {
        FadeDown fade;
        bool lastActive;

    public:
        PulsePattern()
        {
            this->name = "Pulse";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (active && !lastActive)
                fade.reset();
            lastActive = active;

            if (fade.getPhase() == 1)
                return;

            fade.setDuration(params->getVelocity(200, 20));

            RGBA color = params->getSecondaryColor() * fade.getValue();

            for (int index = 0; index < width; index++)
                pixels[index] = color;
        }
    };

    class SlowPulsePattern : public Pattern<RGBA>
    {
        FadeDown fade;
        bool lastActive;

    public:
        SlowPulsePattern()
        {
            this->name = "Slow Pulse";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (active && !lastActive)
                fade.reset();
            lastActive = active;

            if (fade.getPhase() == 1)
                return;

            fade.setDuration(params->getVelocity(3000, 500));

            RGBA color = params->getPrimaryColor() * fade.getValue();

            for (int index = 0; index < width; index++)
                pixels[index] = color;
        }
    };

    class GrowingCirclePattern : public Pattern<RGBA>
    {
        FadeDown fade;
        PixelMap::Polar *map;
        bool lastActive;

    public:
        GrowingCirclePattern(PixelMap::Polar *map)
        {
            this->map = map;
            this->name = "Growing circles";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (active && !lastActive)
                fade.reset();
            lastActive = active;

            float velocity = params->getVelocity(1000, 100);
            fade.duration = params->getSize(400, 50) * velocity / 1000;

            if (fade.isFinished(1 * velocity))
                return;

            for (int i = 0; i < width; i++)
            {
                float fadePosition = fade.getValue(map->r(i) * velocity);
                RGBA color = params->getPrimaryColor();
                pixels[i] = color * fadePosition;
            }
        }
    };

    class LineLaunch : public Pattern<RGBA>
    {
        FadeDown fade = FadeDown(50);
        PixelMap *map;
        bool lastActive;

    public:
        LineLaunch(PixelMap *map)
        {
            this->map = map;
            this->name = "Line launch";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (active && !lastActive)
                fade.reset();
            lastActive = active;

            float velocity = 40;

            if (fade.isFinished(2*velocity))
                return;

            for (int i = 0; i < width; i++)
            {
                float fadePosition = fade.getValue((1 + map->y(i)) * velocity);
                RGBA color = params->getSecondaryColor();
                pixels[i] = color * fadePosition;
            }
        }
    };

}