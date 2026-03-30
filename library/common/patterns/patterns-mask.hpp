
#pragma once
#include "hyperion.hpp"
#include <math.h>
#include <vector>

namespace MaskPatterns
{
    class SinChaseMaskPattern : public Pattern<RGBA>
    {

        Transition transition;
        LFO<Glow> lfo;

    public:
        SinChaseMaskPattern()
        {
            this->name = "Sin chase Mask";
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
                pixels[i] = RGBA(0, 0, 0, 255) * (1. - lfo.getValue(phase)) * transition.getValue();
            }
        }
    };

    class GlowPulseMaskPattern : public Pattern<RGBA>
    {
        Permute perm;
        LFO<Glow> lfo = LFO<Glow>(10000);
        Transition transition;

    public:
        GlowPulseMaskPattern()
        {
            this->name = "Glow pulse mask";
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
                pixels[perm.at[index]] = RGBA(0, 0, 0, 255) * (1. - lfo.getValue(float(index) / width)) * transition.getValue(index, width);
            }
        }
    };

    class SegmentGradientMaskPattern : public Pattern<RGBA>
    {

        Transition transition;
        LFO<Glow> lfo;
        int segmentSize;
        bool direction = false; 

    public:
        SegmentGradientMaskPattern(int segmentSize=60, bool direction = false)
        {
            this->name = "Gradient mask";
            this->segmentSize = segmentSize;
            this->direction = direction;
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            for (int i = 0; i < width; i++)
            {
                float value = (i % segmentSize) / float(segmentSize);
                if (direction) value = 1. - value;
                pixels[i] = RGBA(0, 0, 0, 255) * (1. - value) * transition.getValue();
            }
        }
    };



    class SideChainCompressorMask : public Pattern<RGBA>
    {
        FadeDown fade = FadeDown(500,WaitAtEnd);
        BeatWatcher watcher = BeatWatcher();
        RGBA black = RGBA(0, 0, 0, 255);

    public:
        SideChainCompressorMask()
        {
            this->name = "Side Chain Compressor";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            fade.duration = params->getVelocity(500, 20);
            float depth = params->getAmount(0.2, 0.6);

            if (watcher.Triggered())
                fade.reset();

            auto color = black * fade.getValue() * depth;
            for (int index = 0; index < width; index++)
                pixels[index] = color;
        }
    };


    class SegmentGlitchMaskPattern : public Pattern<RGBA>
    {
        Permute perm = Permute(1);
        Transition transition = Transition(
            0, Transition::none, 0,
            500, Transition::none, 0);
        RGBA black = RGBA(0, 0, 0, 255);

    public:
        SegmentGlitchMaskPattern()
        {
            this->name = "Segment glitch mask";
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

            auto color = black * transition.getValue();

            for (int index = 0; index < width; index++)
            {
                int segmentIndex = index * numSegments / width;

                if (perm.at[segmentIndex] > threshold)
                    pixels[index] = color;
            }
        }
    };

    class RibbenFlashMaskPattern : public Pattern<RGBA>
    {
        Transition transition;
        Permute perm;
        BeatWatcher watcher = BeatWatcher();
        FadeDown fade = FadeDown(2400);
        int segmentSize;
        RGBA black = RGBA(0, 0, 0, 255);

    public:
        RibbenFlashMaskPattern(int segmentSize = 60)
        {
            this->segmentSize = segmentSize;
            this->name = "Ribben flash mask";
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

            RGBA col = black * (1 - fade.getValue()) * transition.getValue();

            for(int i = 0; i < width; i++){
                pixels[i] = col;
            }

            for (int segment = numVisible; segment < numSegments; segment++)
            {
                for (int j = 0; j < segmentSize; j++)
                    pixels[perm.at[segment] * segmentSize + j] = black;
            }
        }
    };
}