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

    inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
    {
        if (!active)
            return;

        //Log::info("ANIMATION_PATTERN","calculate");

        animation->render(
            pixels,
            width,
            *(ledShapes),

            palette,
            params->velocity * 255,
            params->amount * 255,
            params->size * 255,
            params->variant * 255,
            params->offset * 255,

            //todo alpha is already handled by the ControlHubInput, and therefore fixed at 255 here.
            //this param, and the alpha blending in the underlying patterns can be removed.
            255,
            Tempo::GetProgress(4) * 1000,
            1000,
            Tempo::GetBeatNumber(),
            updateCache
        );

        updateCache = false;
    }
};