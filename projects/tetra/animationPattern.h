#pragma once
#include "core/generation/patterns/pattern.hpp"
#include "utils.hpp"
#include "core/generation/pixelMap.hpp"
#include <vector>
#include <math.h>
#include "LedShape.h"
#include "LedAnimation.h"
#include "platform/includes/log.hpp"

class AnimationPattern : public Pattern<RGBA>
{
    std::vector<LedShape> *ledShapes;
    LedAnimation *animation;
    bool updateCache = true;

public:
    AnimationPattern(std::vector<LedShape> *ledShapes, LedAnimation *animation)
    {
        this->ledShapes = ledShapes;
        this->animation = animation;
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
            0,
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