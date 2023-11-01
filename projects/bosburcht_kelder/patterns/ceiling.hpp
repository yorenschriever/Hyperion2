#pragma once
#include "core/generation/patterns/pattern.hpp"
#include "generation/patterns/helpers/fade.h"
#include "generation/patterns/helpers/interval.h"
#include "generation/patterns/helpers/timeline.h"
#include "generation/pixelMap.hpp"
#include "mappingHelpers.hpp"
#include <math.h>
#include <vector>

namespace Ceiling
{

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
        RibbenClivePattern(int segmentSize = 60, int averagePeriod = 10000, float precision = 1)
        {
            this->averagePeriod = averagePeriod;
            this->precision = precision;
            this->segmentSize = segmentSize;
            this->name = "Ribben clive";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            int numSegments = width / segmentSize;
            perm.setSize(numSegments);
            float duty = params->getAmount(0.01, 0.2);
            lfo.setDutyCycle(duty);

            for (int segment = 0; segment < numSegments; segment++)
            {
                int interval = averagePeriod + perm.at[segment] * (averagePeriod * precision) / numSegments / (duty);
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

            float duty = params->getSize(0.1,1);
            float amount = params->getAmount(1,25);
            lfo.setDutyCycle(duty);
            lfo.setSoftEdgeWidth(1./width);
            lfo.setPeriod(velocity * amount);

            for (int i = 0; i < width; i++)
            {
                float v = lfo.getValue(float(i)/width * amount);
                pixels[i] = params->gradient->get(255 * v) * v * transition.getValue();
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
        int beatDivs[5] = {16,8,4,2,2};

    public:
        Fireworks()
        {
            this->name = "Fireworks";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            // int beatDiv = beatDivs[int(params->getAmount(0, 4))];
            // if (this->wachter.Triggered() && Tempo::GetBeatNumber() % beatDiv == 0)
            // {
            //     currentFade = (currentFade + 1) % numFades;
            //     fade[currentFade].reset();
            // }

            if (!transition.Calculate(active))
                return;

            if (watcher.Triggered())
            {
                for(int fade=0;fade<numFades;fade++)
                {
                    centers[fade] = Utils::random(0,width);
                    fades[fade].reset();
                }
            }
                
            for(int fade=0;fade<numFades;fade++)
            {
                fades[fade].setDuration(params->getVelocity(1000,250));
            }

            // float trailSize = params->getSize(5, 20);
            // float velocity = params->getVelocity(250, 5);
            // int offset = params->getOffset(4,9);


            for (int fade=0;fade < numFades; fade++)
            {
                int center = centers[fade];
                int size=60;
                for (int i=0; i<size; i++)
                {
                    float fadeValue = fades[fade].getValue(float(i)/size * 250);
                    if (center+i < width) pixels[center + i] += params->gradient->get(255 * fadeValue) * fadeValue;
                    if (center-1 >= 0) pixels[center - i] += params->gradient->get(255 * fadeValue) * fadeValue;
                }
            }



            // for (int bar = 0; bar < width / segmentSize; bar++)
            // {
            //     for (int i = 0; i < segmentSize; i++)
            //     {
            //         int center = segmentSize / 2;
            //         int distance = abs(center - i);
            //         for (int f = 0; f < numFades; f++)
            //         {
            //             float fadeValue = fade[f].getValue(distance * trailSize + (bar % (width / segmentSize / offset)) * velocity);
            //             pixels[bar * segmentSize + i] += params->gradient->get(255 * fadeValue) * fadeValue * transition.getValue();
            //         }
            //     }
            // }
        }
    };

}



class BarFade
{
public:
    int barSize = 60;
    int center = 30;
    Fade<Down> fade;
    float trailSize = 0.5;

    float getValue(int index)
    {
        int distance = abs(center - index);
        return fade.getValue(distance * trailSize);
    }

    void reset()
    {
        fade.reset();
    }
};

namespace Patterns
{

    // class CeilingChase : public Pattern<RGBA>
    // {
    //     int segmentSize;
    //     Transition transition = Transition(
    //         200, Transition::none, 0,
    //         1000, Transition::none, 0);
    //     LFO<SawDown> lfo;

    // public:
    //     CeilingChase(int segmentSize = 60)
    //     {
    //         this->segmentSize = segmentSize;
    //         this->name = "Ceiling chase";
    //     }

    //     inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    //     {
    //         if (!transition.Calculate(active))
    //             return;

    //         lfo.setPeriod(params->getVelocity(5000, 500));
    //         int multiply = int(params->getAmount(1, 10));
    //         lfo.setDutyCycle(params->getSize());
    //         lfo.setSoftEdgeWidth(1. / width);

    //         for (int i = 0; i < width; i++)
    //             pixels[ceilingMappedIndices[i]] = params->getPrimaryColour() * lfo.getValue(float(i) / width * multiply) * transition.getValue();
    //     }
    // };

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
        int beatDivs[5] = {16,8,4,2,2};

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
                        pixels[bar * segmentSize + i] += params->gradient->get(255 * fadeValue) * fadeValue * transition.getValue();
                    }
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
        int beatDivs[6] = {16,8,4,2,1,1};
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
                for(int i=0;i<48;i++)
                {
                    centers[i] = Utils::random(15,45);
                    delays[i] = Utils::random(0,500);
                }
            }

            if (!transition.Calculate(active))
                return;

            masterFade.duration = Tempo::TimeBetweenBeats() * beatDiv;

            float trailSize = params->getVelocity(15, 5);
            fade.setDuration(params->getSize(1200,300));
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

    // class SinChasePattern : public Pattern<RGBA>
    // {

    //     Transition transition;
    //     LFO<Glow> lfo;

    // public:
    //     SinChasePattern()
    //     {
    //         this->name = "Sin chase";
    //     }

    //     inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    //     {
    //         if (!transition.Calculate(active))
    //             return;

    //         lfo.setDutyCycle(params->getSize(0.1,1));
    //         lfo.setPeriod(params->getVelocity(4000,500));
    //         int amount = params->getAmount(1,5);

    //         for (int i = 0; i < width; i++)
    //         {
    //             float phase = ((float)i/width) * amount;
    //             pixels[ceilingMappedIndices[i]] = params->getSecondaryColour() * lfo.getValue(phase) * transition.getValue();
    //         }
    //     }
    // };


    // class SawChasePattern : public Pattern<RGBA>
    // {

    //     Transition transition;
    //     LFO<SawDown> lfo;

    // public:
    //     SawChasePattern()
    //     {
    //         this->name = "Saw chase";
    //     }

    //     inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    //     {
    //         if (!transition.Calculate(active))
    //             return;

    //         lfo.setDutyCycle(params->getSize(0.1,1));
    //         lfo.setPeriod(params->getVelocity(4000,500));
    //         int amount = 1; //params->getAmount(1,5);
    //         int variantParam = 2; //std::min((int)params->getVariant(1,3),2);

    //         for (int variant = 0; variant < variantParam; variant++)
    //         {
    //             for (int i = 0; i < width; i++)
    //             {
    //                 float phase = ((float)i/width) * amount;
    //                 if (variant == 1) phase  = 1.0 - phase;
    //                 float lfoVal =  lfo.getValue(phase);
    //                 pixels[ceilingMappedIndices[i]] += params->gradient->get(255*lfoVal) * lfoVal * transition.getValue();
    //             }
    //         }
    //     }
    // };

    // class SinChase2Pattern : public Pattern<RGBA>
    // {

    //     Transition transition;
    //     LFO<Glow> lfo;

    // public:
    //     SinChase2Pattern()
    //     {
    //         this->name = "Sin chase 2";
    //     }

    //     inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    //     {
    //         if (!transition.Calculate(active))
    //             return;

    //         lfo.setDutyCycle(params->getSize(0.1,1));
    //         lfo.setPeriod(params->getVelocity(4000,500));
    //         int amount = params->getAmount(1,3);
    //         float offset = params->getOffset(0,5);

    //         for (int i = 0; i < width; i++)
    //         {
    //             float phase = ((float)i/width) * amount * 48 + float(i%(width/2))*offset/width;
    //             pixels[ceilingMappedIndices[i]] = params->getSecondaryColour() * lfo.getValue(phase) * transition.getValue();
    //         }
    //     }
    // };

};