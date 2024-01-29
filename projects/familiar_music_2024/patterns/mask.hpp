#pragma once
#include "core/generation/patterns/pattern.hpp"
#include "generation/patterns/helpers/fade.h"
#include "generation/patterns/helpers/interval.h"
#include "generation/patterns/helpers/timeline.h"
#include "generation/pixelMap.hpp"
#include "mappingHelpers.hpp"
#include <math.h>
#include <vector>

// class BarFade
// {
// public:
//     int barSize = 60;
//     int center = 30;
//     Fade<Down> fade;
//     float trailSize = 0.5;

//     float getValue(int index)
//     {
//         int distance = abs(center - index);
//         return fade.getValue(distance * trailSize);
//     }

//     void reset()
//     {
//         fade.reset();
//     }
// };

namespace Patterns
{

    //     class CeilingChase : public Pattern<RGBA>
    //     {
    //         int segmentSize;
    //         Transition transition = Transition(
    //             200, Transition::none, 0,
    //             1000, Transition::none, 0);
    //         LFO<SawDown> lfo;

    //     public:
    //         CeilingChase(int segmentSize = 60)
    //         {
    //             this->segmentSize = segmentSize;
    //             this->name = "Ceiling chase";
    //         }

    //         inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    //         {
    //             if (!transition.Calculate(active))
    //                 return;

    //             lfo.setPeriod(params->getVelocity(5000, 500));
    //             int multiply = int(params->getAmount(1, 10));
    //             lfo.setDutyCycle(params->getSize());
    //             lfo.setSoftEdgeWidth(1. / width);

    //             for (int i = 0; i < width; i++)
    //                 pixels[ceilingMappedIndices[i]] = params->getPrimaryColour() * lfo.getValue(float(i) / width * multiply) * transition.getValue();
    //         }
    //     };

    //     class FadeFromCenter : public Pattern<RGBA>
    //     {
    //         int segmentSize;
    //         Transition transition = Transition(
    //             200, Transition::none, 0,
    //             1000, Transition::none, 0);
    //         static const int numFades = 3;
    //         int currentFade = 0;
    //         FadeDown fade[numFades];
    //         BeatWatcher wachter;
    //         int beatDivs[5] = {16,8,4,2,2};

    //     public:
    //         FadeFromCenter(int segmentSize = 60)
    //         {
    //             this->segmentSize = segmentSize;
    //             this->name = "Fade from center";
    //         }

    //         inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    //         {
    //             int beatDiv = beatDivs[int(params->getAmount(0, 4))];
    //             if (this->wachter.Triggered() && Tempo::GetBeatNumber() % beatDiv == 0)
    //             {
    //                 currentFade = (currentFade + 1) % numFades;
    //                 fade[currentFade].reset();
    //             }

    //             if (!transition.Calculate(active))
    //                 return;

    //             float trailSize = params->getSize(1, 10);
    //             float velocity = params->getVelocity(250, 5);

    //             for (int bar = 0; bar < width / segmentSize; bar++)
    //             {
    //                 for (int i = 0; i < segmentSize; i++)
    //                 {
    //                     int center = segmentSize / 2;
    //                     int distance = abs(center - i);
    //                     for (int f = 0; f < numFades; f++)
    //                     {
    //                         float fadeValue = fade[f].getValue(distance * trailSize + (bar % (width / segmentSize / 4)) * velocity);
    //                         pixels[bar * segmentSize + i] += params->getGradient(255 * fadeValue) * fadeValue * transition.getValue();
    //                     }
    //                 }
    //             }
    //         }
    //     };

    //     class FadeFromRandom : public Pattern<RGBA>
    //     {
    //         int segmentSize;
    //         Transition transition = Transition(
    //             200, Transition::none, 0,
    //             1000, Transition::none, 0);
    //         FadeDown fade;
    //         BeatWatcher watcher;
    //         int beatDivs[6] = {16,8,4,2,1,1};
    //         int centers[48];
    //         int delays[48];
    //         FadeDown masterFade;

    //     public:
    //         FadeFromRandom(int segmentSize = 60)
    //         {
    //             this->segmentSize = segmentSize;
    //             this->name = "Fade from random";
    //         }

    //         inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    //         {
    //             int beatDiv = beatDivs[int(params->getAmount(0, 5))];
    //             if (this->watcher.Triggered() && Tempo::GetBeatNumber() % beatDiv == 0)
    //             {
    //                 fade.reset();
    //                 masterFade.reset();
    //                 for(int i=0;i<48;i++)
    //                 {
    //                     centers[i] = Utils::random(15,45);
    //                     delays[i] = Utils::random(0,500);
    //                 }
    //             }

    //             if (!transition.Calculate(active))
    //                 return;

    //             masterFade.duration = Tempo::TimeBetweenBeats() * beatDiv;

    //             float trailSize = params->getVelocity(15, 5);
    //             fade.setDuration(params->getSize(1200,300));
    //             float offset = params->getOffset();

    //             for (int bar = 0; bar < width / segmentSize; bar++)
    //             {
    //                 for (int i = 0; i < segmentSize; i++)
    //                 {
    //                     int distance = abs(centers[bar] - i);
    //                     float fadeValue = fade.getValue(distance * trailSize + delays[bar] * offset);
    //                     pixels[bar * segmentSize + i] += params->getPrimaryColour() * fadeValue * masterFade.getValue() * transition.getValue();
    //                 }
    //             }
    //         }
    //     };

    // class SideWave : public Pattern<RGBA>
    //     {
    //         Transition transition = Transition(
    //             200, Transition::none, 0,
    //             1000, Transition::none, 0);
    //         LFO<Glow> lfo;
    //         static const int segmentSize = 60;

    //     public:
    //         SideWave()
    //         {
    //             this->name = "Side wave";
    //         }

    //         inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    //         {
    //             if (!transition.Calculate(active))
    //                 return;

    //             float offset = params->getOffset(0,3);
    //             float duty = params->getAmount(0.1,1);
    //             lfo.setDutyCycle(duty);
    //             lfo.setPeriod(params->getVelocity(3000,500)/duty);
    //             float size = params->getSize();
    //             float baseSize = params->getIntensity(0,10);

    //             for (int bar = 0; bar < width/segmentSize; bar++)
    //             {
    //                 for (int i = 0; i < segmentSize; i++)
    //                 {
    //                     float fadeValue = softEdge(i-baseSize,size * lfo.getValue(2.*float(bar) / (width/segmentSize) * offset)*segmentSize);
    //                     pixels[ceilingMappedIndices[bar * segmentSize + i]] = params->getPrimaryColour() * fadeValue * transition.getValue();
    //                 }
    //             }
    //         }
    //     };

    //     class SinChasePattern : public Pattern<RGBA>
    //     {

    //         Transition transition;
    //         LFO<Glow> lfo;

    //     public:
    //         SinChasePattern()
    //         {
    //             this->name = "Sin chase";
    //         }

    //         inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    //         {
    //             if (!transition.Calculate(active))
    //                 return;

    //             lfo.setDutyCycle(params->getSize(0.1,1));
    //             lfo.setPeriod(params->getVelocity(4000,500));
    //             int amount = params->getAmount(1,5);

    //             for (int i = 0; i < width; i++)
    //             {
    //                 float phase = ((float)i/width) * amount;
    //                 pixels[ceilingMappedIndices[i]] = params->getSecondaryColour() * lfo.getValue(phase) * transition.getValue();
    //             }
    //         }
    //     };

    //     class SawChasePattern : public Pattern<RGBA>
    //     {

    //         Transition transition;
    //         LFO<SawDown> lfo;

    //     public:
    //         SawChasePattern()
    //         {
    //             this->name = "Saw chase";
    //         }

    //         inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    //         {
    //             if (!transition.Calculate(active))
    //                 return;

    //             lfo.setDutyCycle(params->getSize(0.1,1));
    //             lfo.setPeriod(params->getVelocity(4000,500));
    //             int amount = 1; //params->getAmount(1,5);
    //             int variantParam = 2; //std::min((int)params->getVariant(1,3),2);

    //             for (int variant = 0; variant < variantParam; variant++)
    //             {
    //                 for (int i = 0; i < width; i++)
    //                 {
    //                     float phase = ((float)i/width) * amount;
    //                     if (variant == 1) phase  = 1.0 - phase;
    //                     float lfoVal =  lfo.getValue(phase);
    //                     pixels[ceilingMappedIndices[i]] += params->getGradient(255*lfoVal) * lfoVal * transition.getValue();
    //                 }
    //             }
    //         }
    //     };

    class SinChaseMaskPattern : public Pattern<RGBA>
    {

        Transition transition;
        LFO<Glow> lfo;
        uint16_t *remap = nullptr;

    public:
        SinChaseMaskPattern(uint16_t * remap=nullptr)
        {
            this->name = "Sin chase Mask";
            this->remap = remap;
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
                int index=i;
                if (remap) index = remap[index];
                pixels[index] = RGBA(0, 0, 0, 255) * (1. - lfo.getValue(phase)) * transition.getValue();
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

    class SideWaveMask : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<Glow> lfo;
        static const int segmentSize = 60;
        uint16_t *remap = nullptr;

    public:
        SideWaveMask(uint16_t *remap)
        {
            this->name = "Side wave mask";
            this->remap = remap;
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
                    int index = bar * segmentSize + i;
                    if (remap) index = remap[index];
                    pixels[index] = RGBA(0,0,0,255) * (1.-fadeValue) * transition.getValue();
                }
            }
        }
    };

    class RotatingRingsMaskPattern : public Pattern<RGBA>
    {
        Transition transition;
        PixelMap3d *map;
        LFO<Sin> ring1;
        LFO<Sin> ring2;

    public:
        RotatingRingsMaskPattern(PixelMap3d *map)
        {
            this->map = map;
            this->name = "Rotating rings mask";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            auto col1 = params->getPrimaryColour() * transition.getValue();
            auto col2 = params->getSecondaryColour() * transition.getValue();
            float size = params->getSize(0.01, 0.1);
            float zoffset = params->getVariant(0, -0.2);
            ring1.setPeriod(params->getVelocity(20000, 2000));
            ring2.setPeriod(params->getVelocity(14000, 1400));

            for (int index = 0; index < width; index++)
            {
                float z_norm = fromBottom(map->y(index));
                float offset = (map->z(index) + 2) / 2 * params->getOffset();

                pixels[index] = RGBA(0,0,0,255) * (
                    1.
                    -softEdge(abs(z_norm - ring1.getValue(offset)), size) 
                    -softEdge(abs(z_norm - ring2.getValue(offset)), size)
                ) * transition.getValue();
            }
        }
    };

};