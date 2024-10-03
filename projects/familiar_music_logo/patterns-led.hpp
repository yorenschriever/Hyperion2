
#pragma once
#include "colours.h"
#include "core/generation/patterns/helpers/fade.h"
#include "core/generation/patterns/helpers/interval.h"
#include "core/generation/patterns/helpers/timeline.h"
#include "core/generation/patterns/pattern.hpp"
#include "core/generation/pixelMap.hpp"
#include "platform/includes/utils.hpp"
#include <math.h>
#include <vector>

namespace LedPatterns
{
    class OnPattern : public Pattern<RGBA>
    {
        RGB color;

        Transition transition = Transition(
            400, Transition::none, 0,
            400, Transition::none, 0);

    public:
        OnPattern(RGB color, const char *name)
        {
            this->color = color;
            this->name = name;
        }
        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            RGBA value = RGBA(color) * transition.getValue();

            for (int index = 0; index < width; index++)
                pixels[index] = value;
        }
    };

    class PalettePattern : public Pattern<RGBA>
    {
        int colorIndex;

        Transition transition = Transition(
            400, Transition::none, 0,
            400, Transition::none, 0);

    public:
        PalettePattern(int colorIndex, const char *name)
        {
            this->colorIndex = colorIndex;
            this->name = name;
        }
        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            RGBA col;
            if (colorIndex==0) col = params->getPrimaryColour();
            if (colorIndex==1) col = params->getSecondaryColour();

            RGBA value = col * transition.getValue();

            for (int index = 0; index < width; index++)
                pixels[index] = value;
        }
    };

    class GlowPulsePattern : public Pattern<RGBA>
    {
        Permute perm;
        LFO<Glow> lfo = LFO<Glow>(10000);
        Transition transition;

    public:
        GlowPulsePattern()
        {
            this->name = "Glow pulse";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            // float density = 481./width;
            int density2 = width / 481;
            if (width == 96)
                density2 = 10;
            lfo.setPeriod(params->getVelocity(10000, 500));
            lfo.setDutyCycle(0.1);
            perm.setSize(width);

            for (int index = 0; index < width; index++)
            {
                if (index % density2 != 0)
                    continue;
                pixels[perm.at[index]] = params->getSecondaryColour() * lfo.getValue(float(index) / width) * transition.getValue(index, width);
            }
        }
    };

    class SegmentChasePattern : public Pattern<RGBA>
    {
        Transition transition;
        Permute perm;
        LFO<SawDown> lfo = LFO<SawDown>(5000);

    public:
        SegmentChasePattern()
        {
            this->name = "Segment chase";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            int segmentSize = 60;
            int numSegments = width / segmentSize;

            perm.setSize(numSegments);

            float pulseWidth = params->getSize(0.25, 1);
            float factor = params->getAmount(50, 1); // 10; // 1-50;
            lfo.setPeriod(params->getVelocity(2000, 200) * factor);
            lfo.setDutyCycle(pulseWidth / factor);
            float lfoWidth = segmentSize * factor;

            for (int segment = 0; segment < numSegments; segment++)
            {
                int randomSegment = perm.at[segment];

                for (int j = 0; j < segmentSize; j++)
                {
                    float lfoVal = lfo.getValue(float(j) / lfoWidth + float(randomSegment) / numSegments + float(segment));
                    RGBA col = params->getGradient(lfoVal * 255) * lfoVal * transition.getValue();
                    pixels[segment * segmentSize + j] = col;
                }
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

}