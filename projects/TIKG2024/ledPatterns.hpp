#pragma once
#include "core/generation/patterns/pattern.hpp"
#include "generation/patterns/helpers/fade.h"
#include "generation/patterns/helpers/interval.h"
#include "generation/patterns/helpers/timeline.h"
#include "generation/pixelMap.hpp"
#include <math.h>
#include <vector>

inline float softEdge(float dist, float size, float edgeSize = 0.03)
{
    if (dist > size)
        return 0;
    if (dist < size - edgeSize)
        return 1;
    return (size - dist) / edgeSize;
}

namespace Patterns
{

    class BarLFO : public Pattern<RGBA>
    {
        int segmentSize;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<Glow> lfo;

    public:
        BarLFO(int segmentSize = 60)
        {
            this->segmentSize = segmentSize;
            this->name = "Bar LFO";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            int amount = 2 * int(params->getAmount(1, 5));
            lfo.setDutyCycle(params->getSize(0.25, 1));
            lfo.setPeriod(params->getVelocity(1000, 50) * amount);
            lfo.setSoftEdgeWidth(2 * width / segmentSize);

            for (int bar = 0; bar < width; bar += segmentSize)
            {
                for (int i = 0; i < segmentSize; i++)
                    pixels[bar + i] = params->getSecondaryColour() * lfo.getValue(amount * float(bar) / width);
            }
        }
    };

    class GradientLFO : public Pattern<RGBA>
    {
        int segmentSize;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<SawDown> lfo;

    public:
        GradientLFO(int segmentSize = 60)
        {
            this->segmentSize = segmentSize;
            this->name = "Gradient LFO";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            float trailSize = params->getAmount(30, 120);
            lfo.setPeriod(params->getVelocity(4000, 500));
            lfo.setDutyCycle(params->getSize());

            for (int bar = 0; bar < width / segmentSize; bar++)
            {
                for (int i = 0; i < segmentSize; i++)
                {
                    int center = segmentSize / 2;
                    int distance = abs(center - i);

                    float fadeValue = lfo.getValue(float(bar) / (width / segmentSize) * 2 + distance / trailSize);
                    pixels[bar * segmentSize + i] = params->getGradientf(fadeValue) * fadeValue * transition.getValue();
                }
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
                    pixels[bar * segmentSize + i] += params->getPrimaryColour() * fadeValue * masterFade.getValue() * transition.getValue();
                }
            }
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

            float amount = params->getAmount(0.005, 0.1);
            lfo.setPeriod(params->getVelocity(3000, 500) / amount);
            lfo.setDutyCycle(amount);
            perm.setSize(width);

            for (int index = 0; index < width; index++)
            {
                pixels[perm.at[index]] = params->getSecondaryColour() * lfo.getValue(float(index) / width) * transition.getValue(index, width);
            }
        }
    };

    class GlitchPattern : public Pattern<RGBA>
    {
        Permute perm = Permute(0);
        int segmentSize;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        GlitchPattern(int segmentSize = 60)
        {
            this->segmentSize = segmentSize;
            this->name = "Glitch";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            int numSegments = width / segmentSize;
            perm.setSize(numSegments);
            perm.permute();

            RGBA col = params->getPrimaryColour() * transition.getValue();
            for (int segment = 0; segment < numSegments * 0.1; segment++)
            {
                for (int j = 0; j < segmentSize; j++)
                    pixels[perm.at[segment] * segmentSize + j] = col;
            }
        }
    };

    template <class T>
    class RibbenClivePattern : public Pattern<RGBA>
    {
        Transition transition;
        int segmentSize;
        int averagePeriod;
        float precision;
        LFO<T> lfo = LFO<T>();
        Permute perm;

    public:
        RibbenClivePattern(int segmentSize = 60, int averagePeriod = 10000, float precision = 1, float pulsewidth = 0.025)
        {
            this->averagePeriod = averagePeriod;
            this->precision = precision;
            this->segmentSize = segmentSize;
            this->lfo.setDutyCycle(pulsewidth);
            this->name = "Ribben clive";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

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
        int segmentSize;
        Permute perm;
        BeatWatcher watcher = BeatWatcher();
        FadeDown fade = FadeDown(2400);

    public:
        RibbenFlashPattern(int segmentSize = 60)
        {
            this->segmentSize = segmentSize;
            this->name = "Ribben flash";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

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

    class SegmentChasePattern : public Pattern<RGBA>
    {
        Transition transition;
        Permute perm;
        int segmentSize;
        LFO<SawDown> lfo = LFO<SawDown>(5000);

    public:
        SegmentChasePattern(int segmentSize = 60)
        {
            this->segmentSize = segmentSize;
            this->name = "Segment chase";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            int numSegments = width / segmentSize;
            perm.setSize(numSegments);
            float pulseWidth = params->getSize(0.25, 1);
            float factor = params->getAmount(50, 1);
            lfo.setPeriod(params->getVelocity(2000, 200) * factor);
            lfo.setDutyCycle(pulseWidth / factor);
            lfo.setSoftEdgeWidth(1. / width * factor);
            float lfoWidth = segmentSize * factor;

            for (int segment = 0; segment < numSegments; segment++)
            {
                int randomSegment = perm.at[segment];

                for (int j = 0; j < segmentSize; j++)
                {
                    float lfoVal = lfo.getValue(float(j) / lfoWidth + float(randomSegment) / numSegments + float(segment));
                    RGBA col = params->getGradient(lfoVal * 255) * lfoVal * transition.getValue();
                    int index = segment * segmentSize + j;
                    // if (width == sizeof(ceilingMappedIndices) / 2)
                    //     index = ceilingMappedIndices[index];
                    pixels[index] = col;
                }
            }
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

    class CeilingChase : public Pattern<RGBA>
    {
        int segmentSize;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<SawDown> lfo;

    public:
        CeilingChase(int segmentSize = 60)
        {
            this->segmentSize = segmentSize;
            this->name = "Ceiling chase";
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
                pixels[i] = params->getPrimaryColour() * lfo.getValue(float(i) / width * multiply) * transition.getValue();
        }
    };

    class FadeFromCenter : public Pattern<RGBA>
    {
        int segmentSize;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        static const int numFades = 3;
        int currentFade = 0;
        FadeDown fade[numFades];
        BeatWatcher wachter;
        int beatDivs[5] = {16, 8, 4, 2, 2};

    public:
        FadeFromCenter(int segmentSize = 60)
        {
            this->segmentSize = segmentSize;
            this->name = "Fade from center";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            int beatDiv = beatDivs[int(params->getAmount(0, 4))];
            if (this->wachter.Triggered() && Tempo::GetBeatNumber() % beatDiv == 0)
            {
                currentFade = (currentFade + 1) % numFades;
                fade[currentFade].reset();
            }

            if (!transition.Calculate(active))
                return;

            float trailSize = params->getSize(1, 10);
            float velocity = params->getVelocity(250, 5);

            for (int bar = 0; bar < width / segmentSize; bar++)
            {
                for (int i = 0; i < segmentSize; i++)
                {
                    int center = segmentSize / 2;
                    int distance = abs(center - i);
                    for (int f = 0; f < numFades; f++)
                    {
                        float fadeValue = fade[f].getValue(distance * trailSize + (bar % (width / segmentSize / 4)) * velocity);
                        pixels[bar * segmentSize + i] += params->getGradient(255 * fadeValue) * fadeValue * transition.getValue();
                    }
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
        static const int segmentSize = 60;

    public:
        SideWave()
        {
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
                    float fadeValue = softEdge(i - baseSize, size * lfo.getValue(2. * float(bar) / (width / segmentSize) * offset) * segmentSize);
                    pixels[bar * segmentSize + i] = params->getPrimaryColour() * fadeValue * transition.getValue();
                }
            }
        }
    };

    class SinChasePattern : public Pattern<RGBA>
    {

        Transition transition;
        LFO<Glow> lfo;

    public:
        SinChasePattern()
        {
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
                pixels[i] = params->getSecondaryColour() * lfo.getValue(phase) * transition.getValue();
            }
        }
    };

    class SawChasePattern : public Pattern<RGBA>
    {

        Transition transition;
        LFO<SawDown> lfo;

    public:
        SawChasePattern()
        {
            this->name = "Saw chase";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setDutyCycle(params->getSize(0.1, 1));
            lfo.setPeriod(params->getVelocity(4000, 500));
            int amount = params->getAmount(1,5);
            int variantParam = std::min((int)params->getVariant(1,3),2);

            for (int variant = 0; variant < variantParam; variant++)
            {
                for (int i = 0; i < width; i++)
                {
                    float phase = ((float)i / width) * amount;
                    if (variant == 1)
                        phase = 1.0 - phase;
                    float lfoVal = lfo.getValue(phase);
                    pixels[i] += params->getGradient(255 * lfoVal) * lfoVal * transition.getValue();
                }
            }
        }
    };

    class SinChase2Pattern : public Pattern<RGBA>
    {

        Transition transition;
        LFO<Glow> lfo;

    public:
        SinChase2Pattern()
        {
            this->name = "Sin chase 2";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setDutyCycle(params->getSize(0.1, 1));
            lfo.setPeriod(params->getVelocity(4000, 500));
            int amount = params->getAmount(1, 3);
            float offset = params->getOffset(0, 5);

            for (int i = 0; i < width; i++)
            {
                float phase = ((float)i / width) * amount * 48 + float(i % (width / 2)) * offset / width;
                pixels[i] = params->getSecondaryColour() * lfo.getValue(phase) * transition.getValue();
            }
        }
    };

    class Chaser : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<SawDown> lfo;

    public:
        Chaser()
        {
            this->name = "Chaser";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            float velocity = params->getVelocity(250, 50);

            float duty = params->getSize(0.1, 1);
            float amount = params->getAmount(1, 25);
            lfo.setDutyCycle(duty);
            lfo.setSoftEdgeWidth(1. / width);
            lfo.setPeriod(velocity * amount);

            for (int i = 0; i < width; i++)
            {
                float v = lfo.getValue(float(i) / width * amount);
                pixels[i] = params->getGradient(255 * v) * v * transition.getValue();
            }
        }
    };

    class Fireworks : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        static const int numFades = 8;

        int currentFade = 0;
        FadeDown fades[numFades];
        int centers[numFades];
        BeatWatcher watcher;
        int beatDivs[5] = {16, 8, 4, 2, 2};

    public:
        Fireworks()
        {
            this->name = "Fireworks";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            if (watcher.Triggered())
            {
                for (int fade = 0; fade < numFades; fade++)
                {
                    centers[fade] = Utils::random(0, width);
                    fades[fade].reset();
                }
            }

            for (int fade = 0; fade < numFades; fade++)
            {
                fades[fade].setDuration(params->getVelocity(1000, 250));
            }

            for (int fade = 0; fade < numFades; fade++)
            {
                int center = centers[fade];
                int size = 60;
                for (int i = 0; i < size; i++)
                {
                    float fadeValue = fades[fade].getValue(float(i) / size * 250);
                    if (center + i < width)
                        pixels[center + i] += params->getGradient(255 * fadeValue) * fadeValue;
                    if (center - 1 >= 0)
                        pixels[center - i] += params->getGradient(255 * fadeValue) * fadeValue;
                }
            }
        }
    };

    class ColumnChaserTIKG : public Pattern<RGBA>
    {
        int framecounter = 1;
        int j=3;

    public:
        ColumnChaserTIKG()
        {
            this->name = "Column Chase Mapped";
        }

                inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            framecounter--;

            RGBA color = RGBA(0, 0, 0, 255);
            if (framecounter <= 1)
                color = params->getPrimaryColour();
            if (framecounter == 1){
                j++;
                if(j==4){
                    j=0;
                }
            }
            if (framecounter == 0)
                framecounter =  params->getVelocity(40,4);

            for (int index = j*width/4; index < (j+1)*width/4; index++)
                pixels[index] = color;
        }
    };

    class ColumnDimmerTIKG : public Pattern<RGBA>
    {
        int framecounter = 1;
        int j=3;

    public:
        ColumnDimmerTIKG()
        {
            this->name = "Column Dimmer Mapped";
        }

                inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            framecounter--;

            RGBA color = RGBA(0, 0, 0, 255);
            if (framecounter <= 1)
                color = params->getSecondaryColour();
            
            if (framecounter == 0)
                framecounter =  1;
                j = 3*params->getSize();

           

            for (int index = j*width/4; index < (j+1)*width/4; index++)
                pixels[index] = color;
        }
    };

};