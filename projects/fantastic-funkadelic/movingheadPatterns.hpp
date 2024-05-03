#pragma once
#include "core/generation/patterns/helpers/fade.h"
#include "core/generation/patterns/helpers/interval.h"
#include "core/generation/patterns/helpers/timeline.h"
#include "core/generation/patterns/pattern.hpp"
#include "core/generation/pixelMap.hpp"
#include "platform/includes/utils.hpp"
#include "utils.hpp"
#include <math.h>
#include <vector>

namespace MovingheadPatterns
{

    class WallPattern : public Pattern<MovingHead>
    {
        Transition trans = Transition(200, 400);
        LFO<SinFast> lfo = LFO<SinFast>(2000);

    public:
        WallPattern()
        {
            this->name = "Wall";
        }

        inline void Calculate(MovingHead *pixels, int width, bool active, Params *params) override
        {
            if (!trans.Calculate(active))
                return;

            for (int index = 0; index < width; index++)
            {
                RGBA col = index % 2 == 0 ? params->getPrimaryColour() : params->getSecondaryColour();
                pixels[index] += MovingHead(0, -30, col * trans.getValue() * (0.25 + 0.75 * lfo.getValue(float(index) / width)));
            }
        }
    };

    // TODO
    class WallBeatPattern : public Pattern<MovingHead>
    {
        Transition trans = Transition(200, 400);
        LFO<SinFast> lfo = LFO<SinFast>(2000);
        BeatWatcher tw;

    public:
        WallBeatPattern()
        {
            this->name = "Wall beat";
        }

        inline void Calculate(MovingHead *pixels, int width, bool active, Params *params) override
        {
            if (!trans.Calculate(active))
                return;

            for (int index = 0; index < width; index++)
            {
                RGBA col = index % 2 == 0 ? params->getPrimaryColour() : params->getSecondaryColour();
                pixels[index] += MovingHead(0, -30, col * trans.getValue() * (0.25 + 0.75 * lfo.getValue()));
            }
        }
    };

    class Flash360Pattern : public Pattern<MovingHead>
    {
        LFO<SinFast> lfopan = LFO<SinFast>(10000);
        LFO<SinFast> lfotilt = LFO<SinFast>(8000);
        BeatWatcher tw;
        Permute perm;
        FadeDown fade = FadeDown(300);

    public:
         Flash360Pattern()
        {
            this->name = "Beat Flash 360";
        }

        inline void Calculate(MovingHead *pixels, int width, bool active, Params *params) override
        {
            perm.setSize(width);
            if (tw.Triggered() && active)
            {
                perm.permute();
                fade.reset();
            }

            if (!active && fade.getValue() == 0)
                return;

            for (int index = 0; index < width; index++)
            {
                float pan = lfopan.getValue((float)index / width) * 300 - 150;
                float tilt = lfotilt.getValue((float)index / width) * 160 - 80;
                RGBA col = perm.at[index] == 0 ? params->getPrimaryColour() : RGBA(0, 0, 0, 0);
                pixels[index] += MovingHead(pan, tilt, col * fade.getValue());
            }
        }
    };

    class WallDJPattern : public Pattern<MovingHead>
    {
        LFO<SinFast> lfo = LFO<SinFast>(10000);
        Transition trans = Transition(200, 400);

    public:
        WallDJPattern()
        {
            this->name = "Wall DJ";
        }

        inline void Calculate(MovingHead *pixels, int width, bool active, Params *params) override
        {
            if (!trans.Calculate(active))
                return;

            for (int index = 0; index < width; index++)
            {
                RGBA col = index % 2 == 0 ? params->getPrimaryColour() : params->getSecondaryColour();
                float tilt = index % 2 == 0 ? -30 : 60;
                pixels[index] += MovingHead(0, tilt, col * lfo.getValue(float(index % 2) / 2) * trans.getValue());
            }
        }
    };

    class DJPattern : public Pattern<MovingHead>
    {
        LFO<SinFast> lfo = LFO<SinFast>(10000);
        Transition trans = Transition(200, 400);

    public:
        DJPattern()
        {
            this->name = "DJ";
        }

        inline void Calculate(MovingHead *pixels, int width, bool active, Params *params) override
        {
            if (!trans.Calculate(active))
                return;

            for (int index = 0; index < width; index++)
            {
                RGBA col = index % 2 == 0 ? RGBA(0,0,0,0) : params->getSecondaryColour();
                float tilt = index % 2 == 0 ? -30 : 60;
                pixels[index] += MovingHead(0, tilt, col * lfo.getValue(float(index % 2) / 2) * trans.getValue());
            }
        }
    };

    class FrontPattern : public Pattern<MovingHead>
    {
        Transition trans = Transition(200, 400);
        LFO<SinFast> lfo = LFO<SinFast>(1000);

    public:
        FrontPattern()
        {
            this->name = "Front";
        }

        inline void Calculate(MovingHead *pixels, int width, bool active, Params *params) override
        {
            if (!trans.Calculate(active))
                return;

            for (int index = 0; index < width; index++)
            {
                RGBA col = index % 2 == 0 ? params->getPrimaryColour() : params->getSecondaryColour();
                pixels[index] += MovingHead(0, 60, col * lfo.getValue((float)index / width) * trans.getValue());
            }
        }
    };

    class SidesPattern : public Pattern<MovingHead>
    {
        Transition trans = Transition(200, 600);
        LFO<SinFast> lfo = LFO<SinFast>(5000);
        LFO<SinFast> lfotilt = LFO<SinFast>(4000);

    public:
        SidesPattern()
        {
            this->name = "Sides";
        }

        inline void Calculate(MovingHead *pixels, int width, bool active, Params *params) override
        {
            if (!trans.Calculate(active))
                return;

            for (int index = 0; index < width; index++)
            {
                RGBA col = index % 2 == 0 ? params->getPrimaryColour() : params->getSecondaryColour();
                pixels[index] += MovingHead(
                    (index - width / 2) * 40 - 40 + 40 * lfotilt.getValue(0.25),
                    20 + 40 * lfotilt.getValue(),
                    col * (0.1 + 0.9 * lfo.getValue((float)index / width)) * trans.getValue());
            }
        }
    };

    class GlitchPattern : public Pattern<MovingHead>
    {
        LFO<SinFast> lfopan = LFO<SinFast>(5000);
        LFO<SinFast> lfotilt = LFO<SinFast>(2000);

    public:
        GlitchPattern()
        {
            this->name = "Glitch";
        }

        inline void Calculate(MovingHead *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            for (int index = 0; index < width; index++)
            {
                float pan = lfopan.getValue((float)index / width) * 300 - 150;
                float tilt = lfotilt.getValue((float)index / width) * 160 - 80;
                pixels[index] += MovingHead(pan, tilt, Utils::millis() % 100 < 25 ? params->getSecondaryColour() : RGBA(0, 0, 0, 255), 0);
            }
        }
    };

    class UVPattern : public Pattern<MovingHead>
    {

    public:
        UVPattern()
        {
            this->name = "UV";
        }

        inline void Calculate(MovingHead *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            for (int index = 0; index < width; index++)
            {
                pixels[index] += MovingHead(0, -45, RGBA(0, 0, 0, 255), Utils::millis() % 50 < 25 ? 255 : 0);
            }
        }
    };

}