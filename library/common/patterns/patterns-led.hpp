
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
                col = params->getPrimaryColor();
            if (colorIndex == 1)
                col = params->getSecondaryColor();

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
                pixels[perm.at[index]] = params->getSecondaryColor() * lfo.getValue(float(index) / width) * transition.getValue(index, width);
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
                color = params->getSecondaryColor() + RGBA(255, 255, 255, 255) * ((val - 0.5) * 2.);
            else
                color = params->getSecondaryColor() * (val * 2.);

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
                color = params->getPrimaryColor();

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
        int thresholdDiv;
        ;

    public:
        PixelGlitchPattern(int thresholdDiv = 100)
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

            int threshold = float(width) / params->getAmount(thresholdDiv, 2);

            for (int index = 0; index < threshold; index++)
                pixels[perm.at[index]] = params->getSecondaryColor() * transition.getValue();
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
                pixels[perm.at[index]] = params->getHighlightColor();
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
                color = params->getHighlightColor();

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
                pixels[index] = params->getSecondaryColor() * lfo.getValue(amount * float(index) / width) * transition.getValue(index, width);
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
                    pixels[perm.at[index]] = params->getPrimaryColor() * lfo.getValue(float(Transition::fromCenter(index, width, 1000)) / -1000);
                else
                    pixels[perm.at[index]] = params->getPrimaryColor() + (params->getSecondaryColor() * (val - 1) / 0.025);
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
                RGBA col = params->getPrimaryColor() * lfo.getValue(0, interval) * transition.getValue();
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
                RGBA col = params->getPrimaryColor() * fade.getValue() * transition.getValue();
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
                pixels[index] += params->getHighlightColor() * transition.getValue();
            }
        }
    };

    template <class T>
    class ClivePattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        int numSegments;
        int averagePeriod;
        float precision;
        LFO<T> lfo;
        Permute perm;

    public:
        ClivePattern(int numSegments = 10, int averagePeriod = 1000, float precision = 1, float pulsewidth = 1)
        {
            this->numSegments = std::max(numSegments, 1);
            this->averagePeriod = averagePeriod;
            this->precision = precision;
            this->perm = Permute(numSegments);
            this->lfo.setDutyCycle(pulsewidth);
            this->name = "Clive pattern";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            for (int index = 0; index < width; index++)
            {
                int permutedQuantized = perm.at[index * numSegments / width] * width / numSegments;
                int interval = averagePeriod + permutedQuantized * (averagePeriod * precision) / width;
                pixels[index] += params->getSecondaryColor() * lfo.getValue(0, interval) * transition.getValue();
            }
        }
    };

    class BarChase : public Pattern<RGBA>
    {
        int segmentSize;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<SawDown> lfo;

    public:
        BarChase(int segmentSize = 60)
        {
            this->segmentSize = segmentSize;
            this->name = "Bar chase";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(params->getVelocity(5000, 500));
            int multiply = int(params->getAmount(1, 10));
            lfo.setDutyCycle(params->getSize());
            lfo.setSoftEdgeWidth(1. / width);

            for (int i = 0; i < width; i++)
            {
                pixels[i] = params->getPrimaryColor() * lfo.getValue(float(i) / width * multiply) * transition.getValue();
            }
        }
    };

    class FadeFromRandom : public Pattern<RGBA>
    {
        int segmentSize;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        FadeDown fade;
        BeatWatcher watcher;
        int beatDivs[6] = {16, 8, 4, 2, 1, 1};
        int centers[48];
        int delays[48];
        FadeDown masterFade;

    public:
        FadeFromRandom(int segmentSize = 60)
        {
            this->segmentSize = segmentSize;
            this->name = "Fade from random";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            int beatDiv = beatDivs[int(params->getAmount(0, 5))];
            if (this->watcher.Triggered() && Tempo::GetBeatNumber() % beatDiv == 0)
            {
                fade.reset();
                masterFade.reset();
                for (int i = 0; i < 48; i++)
                {
                    centers[i] = Utils::random(15, 45);
                    delays[i] = Utils::random(0, 500);
                }
            }

            if (!transition.Calculate(active))
                return;

            masterFade.duration = Tempo::TimeBetweenBeats() * beatDiv;

            float trailSize = params->getVelocity(15, 5);
            fade.setDuration(params->getSize(1200, 300));
            float offset = params->getOffset();

            for (int bar = 0; bar < width / segmentSize; bar++)
            {
                for (int i = 0; i < segmentSize; i++)
                {
                    int distance = abs(centers[bar] - i);
                    float fadeValue = fade.getValue(distance * trailSize + delays[bar] * offset);
                    pixels[bar * segmentSize + i] += params->getPrimaryColor() * fadeValue * masterFade.getValue() * transition.getValue();
                }
            }
        }
    };

class SideWave : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<Glow> lfo;
        int segmentSize = 60;

    public:
        SideWave(int segmentSize = 60)
        {
            this->segmentSize = segmentSize;
            this->name = "Side wave";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            float offset = params->getOffset(0, 3);
            float duty = params->getAmount(0.1, 1);
            lfo.setDutyCycle(duty);
            lfo.setPeriod(params->getVelocity(3000, 500) / duty);
            float size = params->getSize();
            float baseSize = params->getIntensity(0, 10);

            for (int bar = 0; bar < width / segmentSize; bar++)
            {
                for (int i = 0; i < segmentSize; i++)
                {
                    int index = bar * segmentSize + i;
                    float fadeValue = softEdge(i - baseSize, size * lfo.getValue(2. * float(bar) / (width / segmentSize) * offset) * segmentSize);
                    pixels[index] = params->getPrimaryColor() * fadeValue * transition.getValue();
                }
            }
        }
    };

    class SinChasePattern : public Pattern<RGBA>
    {
        Transition transition;
        LFO<Glow> lfo;
        int segmentSize = 60;

    public:
        SinChasePattern(int segmentSize = 60)
        {
            this->segmentSize = segmentSize;
            this->name = "Sin chase";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setDutyCycle(params->getSize(0.1, 1));
            lfo.setPeriod(params->getVelocity(4000, 500));
            int amount = params->getAmount(1, 5);

            for (int i = 0; i < width; i++)
            {
                float phase = ((float)i / width) * amount;
                pixels[i] = params->getSecondaryColor() * lfo.getValue(phase) * transition.getValue();
            }
        }
    };


   
}
