#pragma once
// #include "core/generation/patterns/pattern.hpp"
// #include "generation/patterns/helpers/fade.h"
// #include "generation/patterns/helpers/interval.h"
// #include "generation/patterns/helpers/timeline.h"
// #include "generation/pixelMap.hpp"

#include <math.h>
#include <vector>

namespace Flash
{
    class FadingNoisePattern : public Pattern<RGBA>
    {
        Fade<Down, Cubic> fade = Fade<Down, Cubic>(600);
        Permute perm;

    public:
        FadingNoisePattern()
        {
            this->name = "Fading noise";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
            {
                fade.reset();
                return;
            }

            perm.setSize(width);
            perm.permute();

            int noiseLevel = fade.getValue() * width / 3;
            for (int index = 0; index < noiseLevel; index++)
            {
                pixels[perm.at[index]] = params->getHighlightColour();
            }
        }
    };

    class SquareGlitchPattern : public Pattern<RGBA>
    {
        // Timeline timeline = Timeline(50);
        Permute perm = Permute(256);
        Transition transition = Transition(
            0, Transition::none, 0,
            500, Transition::none, 0);

        PixelMap3d *map;

    public:
        SquareGlitchPattern(PixelMap3d *map)
        {
            this->map = map;
            this->name = "Square glitch";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            int numCubes = params->getSize(2, 15);
            int numCubes3d = numCubes * numCubes * numCubes;
            int threshold = params->getAmount(1, numCubes3d/5);
            perm.setSize(numCubes3d);
            perm.permute();

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                int xquantized = Utils::rescale(map->x(index),0,numCubes,-1,1);
                int yquantized = Utils::rescale(map->y(index),0,numCubes,-0.5,.5);
                int zquantized = Utils::rescale(map->z(index),0,numCubes,-1,1);

                int cubeIndex = 
                    xquantized + 
                    zquantized * numCubes +
                    yquantized * numCubes * numCubes;

                if (perm.at[cubeIndex] > threshold )
                    continue;
                pixels[index] += params->getHighlightColour() * transition.getValue();
            }
        }
    };

    class FlashesPattern : public Pattern<RGBA>
    {
        FadeDown fade = FadeDown(2400);
        BeatWatcher watcher = BeatWatcher();

    public:
        FlashesPattern()
        {
            this->name = "Flashes";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;
            fade.duration = params->getVelocity(1500, 100);

            if (watcher.Triggered())
                fade.reset();

            RGBA color;
            float val = fade.getValue();
            if (val >= 0.5)
                color = params->getSecondaryColour() + RGBA(255, 255, 255, 255) * ((val - 0.5) * 2.);
            else
                color = params->getSecondaryColour() * (val * 2.);

            for (int index = 0; index < width; index++)
                pixels[index] = color;
        }
    };

    class StrobePattern : public Pattern<RGBA>
    {
        int framecounter = 1;

    public:
        StrobePattern()
        {
            this->name = "Strobe palette";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            framecounter--;

            RGBA color = RGBA(0, 0, 0, 255);
            if (framecounter <= 1)
                color = params->getPrimaryColour();

            if (framecounter == 0)
                framecounter = 5; // params->getVelocity(40,4);

            for (int index = 0; index < width; index++)
                pixels[index] = color;
        }
    };

    class StrobeHighlightPattern : public Pattern<RGBA>
    {
        int framecounter = 1;

    public:
        StrobeHighlightPattern()
        {
            this->name = "Strobe white";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            framecounter--;

            RGBA color = RGBA(0, 0, 0, 255);
            if (framecounter <= 1)
                color = params->getHighlightColour();

            if (framecounter == 0)
                framecounter = 5; // params->getVelocity(40,4);

            for (int index = 0; index < width; index++)
                pixels[index] = color;
        }
    };

    class PixelGlitchPattern : public Pattern<RGBA>
    {
        Timeline timeline = Timeline(50);
        Permute perm = Permute(0);
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        PixelGlitchPattern()
        {
            this->name = "Pixel glitch";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            timeline.FrameStart();
            perm.setSize(width);

            if (timeline.Happened(0))
                perm.permute();

            for (int index = 0; index < width / 30; index++)
                pixels[perm.at[index]] = params->getSecondaryColour() * transition.getValue();
        }
    };

    // class PetalGlitchPattern : public Pattern<RGBA>
    // {
    //     Transition transition = Transition(
    //         200, Transition::none, 0,
    //         1000, Transition::none, 0);
    //     Timeline timeline = Timeline(1000);
    //     int petal = 0;

    // public:
    //     PetalGlitchPattern()
    //     {
    //         this->name = "Petal glitch";
    //     }

    //     inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    //     {
    //         if (!transition.Calculate(active))
    //             return;

    //         // i made a timelineline longer than 60, and reset it here manually,
    //         // so it resets exactly during this frame instead of freely cycling at its own pace
    //         timeline.FrameStart();
    //         if (timeline.Happened(60))
    //         {
    //             timeline.reset();
    //             petal = (petal + 1 + Utils::random(0, 4)) % 6;
    //         }

    //         RGBA col = params->getSecondaryColour() * transition.getValue();
    //         for (int j = 0; j < 45; j++)
    //             pixels[LedsterShapes::petals[petal][j]] += col;
    //     }
    // };

    class StrobeFadePattern : public Pattern<RGBA>
    {
        FadeDown fade = FadeDown(1500);
        PixelMap3d::Spherical *map;

    public:
        StrobeFadePattern(PixelMap3d::Spherical *map)
        {
            this->map = map;
            this->name = "Strobe fade noise";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
            {
                fade.reset();
                return;
            }

            for (int index = 0; index < width; index++)
            {
                float fadePos = Utils::rescale(map->th(index), 0, 3000, 0, 2* M_PI);
                float fadeValue = Utils::rescale_c(fade.getValue(fadePos),0,1, 0, 0.5);
                RGBA col = Utils::millis() % 100 < 25 ? params->getHighlightColour() : RGBA(0,0,0,255);
                pixels[index] = col * fadeValue;
            }
        }
    };

}