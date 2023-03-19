#pragma once
#include "core/generation/patterns/pattern.hpp"
#include "utils.hpp"
#include "core/generation/pixelMap.hpp"
#include <vector>
#include <math.h>
#include "LedShape.h"
#include "LedAnimation.h"
#include "generation/patterns/helpers/palette.hpp"
#include "platform/includes/log.hpp"

class AnimationPattern : public Pattern<RGBA>
{
    std::vector<LedShape> *ledShapes;
    LedAnimation *animation;
    Palette *palette;
    bool updateCache = true;

public:
    AnimationPattern(std::vector<LedShape> *ledShapes, LedAnimation *animation, Palette *palette)
    {
        this->ledShapes = ledShapes;
        this->animation = animation;
        this->palette = palette;
    }

    inline void Calculate(RGBA *pixels, int width, bool active) override
    {
        if (!active)
            return;

        //Log::info("ANIMATION_PATTERN","calculate");

        animation->render(
            pixels,
            width,
            *(ledShapes),

            //todo read those parameters from Params or midi inputs
            palette,
            127,
            64,
            127,
            64,
            32,
            255,
            Utils::millis() % 1000,
            1000,
            Utils::millis() / 1000,
            updateCache
        );

        updateCache = false;
    }
};