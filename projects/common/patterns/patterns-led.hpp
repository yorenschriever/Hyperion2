
#pragma once
#include "hyperion.hpp"
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
            if (colorIndex == 0)
                col = params->getPrimaryColour();
            if (colorIndex == 1)
                col = params->getSecondaryColour();

            RGBA value = col * transition.getValue();

            for (int index = 0; index < width; index++)
                pixels[index] = value;
        }
    };

    class GlowPattern : public Pattern<RGBA>
    {
        Permute perm;
        LFO<Glow> lfo;
        Transition transition;

    public:
        GlowPattern()
        {
            this->name = "Glow";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            float amount = params->getAmount();
            float velocity = params->getVelocity(10000, 500);

            lfo.setPeriod(velocity / amount);
            lfo.setDutyCycle(amount);
            perm.setSize(width);

            for (int index = 0; index < width; index++)
            {
                pixels[perm.at[index]] = params->getSecondaryColour() * lfo.getValue(float(index) / width) * transition.getValue(index, width);
            }
        }
    };

    class SegmentChasePattern : public Pattern<RGBA>
    {
        Transition transition;
        Permute perm;
        int segmentSize;
        LFO<SawDown> lfo = LFO<SawDown>(5000);

    public:
        SegmentChasePattern(int segmentSize = 60)
        {
            this->name = "Segment chase";
            this->segmentSize = segmentSize;
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

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
        int thresholdDiv;;

    public:
        PixelGlitchPattern(int thresholdDiv = 2)
        {
            this->name = "Pixel glitch";
            this->thresholdDiv = thresholdDiv;
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            timeline.FrameStart();
            perm.setSize(width);

            if (timeline.Happened(0))
                perm.permute();

            int threshold = float(width)/params->getAmount(thresholdDiv, 1);

            for (int index = 0; index < threshold; index++)
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

    class SinPattern : public Pattern<RGBA>
    {
        LFO<NegativeCosFast> lfo;
        Transition transition = Transition(
            1000, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        SinPattern()
        {
            this->name = "Sin";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            float velocity = params->getVelocity(20000, 2000);
            int amount = params->getAmount(1, 3.99);
            float size = params->getSize(0.05, 0.5);

            lfo.setPeriod(velocity * size * amount);
            lfo.setDutyCycle(size);

            for (int index = 0; index < width; index++)
                pixels[index] = params->getSecondaryColour() * lfo.getValue(amount * float(index) / width) * transition.getValue(index, width);
        }
    };

    class GradientChasePattern : public Pattern<RGBA>
    {
        Transition transition;
        LFO<SawDown> lfo = LFO<SawDown>(5000);

    public:
        GradientChasePattern()
        {
            this->name = "Gradient chase";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            float velocity = params->getVelocity(6000, 500);
            int amount = params->getAmount(1, 2.99);
            float size = params->getSize();

            lfo.setPeriod(velocity / size * amount);
            lfo.setDutyCycle(size);

            for (int i = 0; i < width; i++)
            {
                float lfoVal = lfo.getValue(amount * float(i) / width);
                pixels[i] = params->getGradient(lfoVal * 255) * lfoVal * transition.getValue();
            }
        }
    };

    class GlowPulsePattern : public Pattern<RGBA>
    {
        Permute perm;
        LFO<Glow> lfo;
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

            lfo.setPeriod(500 + 10000 * (1.0f - params->getVelocity()));
            lfo.setDutyCycle(params->getAmount());
            perm.setSize(width);

            for (int index = 0; index < width; index++)
            {
                float val = 1.025 * lfo.getValue(float(Transition::fromCenter(index, width, 1000)) / -1000);
                if (val < 1.0)
                    pixels[perm.at[index]] = params->getPrimaryColour() * lfo.getValue(float(Transition::fromCenter(index, width, 1000)) / -1000);
                else
                    pixels[perm.at[index]] = params->getPrimaryColour() + (params->getSecondaryColour() * (val - 1) / 0.025);
            }
        }
    };

    template <class T>
    class RibbenClivePattern : public Pattern<RGBA>
    {
        Transition transition;
        const int segmentSize = 60;
        int averagePeriod = 10000;
        float precision = 1;
        LFO<T> lfo = LFO<T>();
        Permute perm;

    public:
        RibbenClivePattern(int averagePeriod = 10000, float precision = 1, float pulsewidth = 0.025)
        {
            this->averagePeriod = averagePeriod;
            this->precision = precision;
            this->lfo.setDutyCycle(pulsewidth);
            this->name = "Ribben clive";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            int segmentSize = 60;
            int numSegments = width / segmentSize;

            perm.setSize(numSegments);
            lfo.setDutyCycle(params->getAmount(0.1, 0.5));

            for (int segment = 0; segment < numSegments; segment++)
            {
                int interval = averagePeriod + perm.at[segment] * (averagePeriod * precision) / numSegments;
                RGBA col = params->getPrimaryColour() * lfo.getValue(0, interval) * transition.getValue();
                for (int j = 0; j < segmentSize; j++)
                    pixels[segment * segmentSize + j] = col;
            }
        }
    };

    class RibbenFlashPattern : public Pattern<RGBA>
    {
        Transition transition;
        Permute perm;
        BeatWatcher watcher = BeatWatcher();
        FadeDown fade = FadeDown(2400);

    public:
        RibbenFlashPattern()
        {
            this->name = "Ribben flash";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            int segmentSize = 60;
            int numSegments = width / segmentSize;
            perm.setSize(numSegments);

            if (watcher.Triggered())
            {

                perm.permute();
                fade.reset();
            }

            fade.duration = params->getVelocity(2500, 100);
            int numVisible = params->getAmount(1, numSegments / 3);

            for (int ribbe = 0; ribbe < numVisible; ribbe++)
            {
                RGBA col = params->getPrimaryColour() * fade.getValue() * transition.getValue();
                for (int j = 0; j < segmentSize; j++)
                    pixels[perm.at[ribbe] * segmentSize + j] += col;
            }
        }
    };

    class SegmentGlitchPattern : public Pattern<RGBA>
    {
        // Timeline timeline = Timeline(50);
        Permute perm = Permute(1);
        Transition transition = Transition(
            0, Transition::none, 0,
            500, Transition::none, 0);

    public:
        SegmentGlitchPattern()
        {
            this->name = "Segment glitch";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            int numSegmentsPerBar = 30 - params->getSize(29, 15);
            int numSegments = (width / 60) * numSegmentsPerBar;
            int threshold = params->getAmount(0, numSegments / 8);
            perm.setSize(numSegments);
            perm.permute();

            for (int index = 0; index < width; index++)
            {
                int segmentIndex = index * numSegments / width;

                if (perm.at[segmentIndex] > threshold)
                    continue;
                pixels[index] += params->getHighlightColour() * transition.getValue();
            }
        }
    };
}
