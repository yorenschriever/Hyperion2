#pragma once
#include "../animation.hpp"
#include "core/generation/patterns/pattern.hpp"
#include "generation/patterns/helpers/fade.h"
#include "generation/patterns/helpers/interval.h"
#include "generation/patterns/helpers/timeline.h"
#include "generation/pixelMap.hpp"
#include <math.h>
#include <vector>

class BgPattern : public Pattern<RGBA>
{

public:
    BgPattern()
    {
        this->name = "BG";
    }

    inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    {
        if (!active)
            return;

        for (int i = 0; i < width; i++)
            pixels[i] = params->getSecondaryColour();
    }
};

class VideoPattern : public Pattern<RGBA>
{
    Transition transition = Transition(
        200, Transition::none, 0,
        1000, Transition::none, 0);
    int frame = 0;
    Animation *animation;

public:
    VideoPattern(Animation *animation)
    {
        this->name = "Video";
        this->animation = animation;
    }

    inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    {
        if (!transition.Calculate(active))
            return;

        frame++;
        if (frame >= animation->frames)
            frame = 0;

        for (int i = 0; i < width; i++)
        {
            int animationIndex = (frame * animation->pixels + i) * animation->depth;
            if (animationIndex+2 >= animation->pixelData.size())
                return;

            pixels[i] = (RGBA)RGB(
                            animation->pixelData[animationIndex + 0],
                            animation->pixelData[animationIndex + 1],
                            animation->pixelData[animationIndex + 2]) *
                        transition.getValue();
            if (animation->depth==4)
                pixels[i] = pixels[i] * (float(animation->pixelData[animationIndex + 3])/255.);
        }
    }
};

class VideoPalettePattern : public Pattern<RGBA>
{
    Transition transition = Transition(
        200, Transition::none, 0,
        1000, Transition::none, 0);
    int frame = 0;
    Animation *animation;

public:
    VideoPalettePattern(Animation *animation, const char* name)
    {
        this->name = name;
        this->animation = animation;
    }

    inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    {
        if (!transition.Calculate(active))
            return;

        frame++;
        if (frame >= animation->frames)
            frame = 0;

        for (int i = 0; i < width; i++)
        {
            int animationIndex = (frame * animation->pixels + i) * animation->depth;
            if (animationIndex+2 >= animation->pixelData.size())
                return;

            pixels[i] += params->getPrimaryColour() * (float(animation->pixelData[animationIndex + 0]) / 255.);
            pixels[i] += params->getSecondaryColour() * (float(animation->pixelData[animationIndex + 1]) / 255.);
            //pixels[i] += params->getHighlightColour() * (float(animation->pixelData[animationIndex + 2]) / 255.);
            pixels[i] += params->getGradientf(0.25) * (float(animation->pixelData[animationIndex + 2]) / 255.);

            pixels[i] = pixels[i] * transition.getValue();
        }
    }
};