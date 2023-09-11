#pragma once
#include "core/generation/patterns/pattern.hpp"
#include "utils.hpp"
#include "core/generation/pixelMap.hpp"
#include <vector>
#include <math.h>
#include "LedShape.h"
#include "LedAnimation.h"
#include "gradient.hpp"
#include "platform/includes/log.hpp"
#include <string>

class AnimationPattern : public Pattern<RGBA>
{
    std::vector<LedShape> *ledShapes;
    LedAnimation *animation;

public:
    AnimationPattern(std::vector<LedShape> *ledShapes, LedAnimation *animation, std::string name)
    {
        this->ledShapes = ledShapes;
        this->animation = animation;
        this->name = name;
    }

    inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
    {
        if (!active)
            return;

        //Log::info("ANIMATION_PATTERN","calculate");

        animation->render(
            pixels,
            width,
            *(ledShapes),

            params->gradient,
            params->velocity * 255,
            params->amount * 255,
            params->size * 255,
            params->variant * 255,
            params->offset * 255,

            //todo alpha is already handled by the ControlHubInput, and therefore fixed at 255 here.
            //this param, and the alpha blending in the underlying patterns can be removed.
            255,
            //Tempo::GetProgress(4) * 1000,
            Utils::millis()%1000,
            1000,
            //Tempo::GetBeatNumber(),
            Utils::millis() / 1000,
            true //always update cache. we got cpu cycles like its 2023
        );

    }
};