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

namespace Chevrons
{

template <class T>
    class RibbenClivePattern : public Pattern<RGBA>
    {
        Transition transition;
        const int segmentSize = 60;
        int averagePeriod=10000;
        float precision=1;
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

            // int numSegments = width / segmentSize;
            perm.setSize(numSegments);

            // for (int ribbe = 0; ribbe < numSegments; ribbe++)
            // {
            //     int interval = averagePeriod + perm.at[ribbe] * (averagePeriod * precision) / numSegments;
            //     RGBA col = params->getPrimaryColour() * lfo.getValue(0, interval);
            //     for (int j = 0; j < segmentSize; j++)
            //     {
            //         pixels[ribbe * segmentSize + j] = col;
            //     }
            // }

            lfo.setDutyCycle(params->getAmount(0.1, 0.5));
            //lfo.setPeriod(params->getVelocity(10000,500));

            for (int segment = 0; segment < numSegments; segment++)
            {
                // int randomSegment = perm.at[segment];
                // RGBA col = params->getPrimaryColour() * lfo.getValue(float(randomSegment)/numSegments);
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
        RibbenFlashPattern() {
            this->name = "Ribben flash";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            int segmentSize =  60;
            int numSegments = width / segmentSize;
            perm.setSize(numSegments);

            if (watcher.Triggered())
            {

                perm.permute();
                fade.reset();
            }

            fade.duration = params->getVelocity(2500, 100);
            int numVisible = params->getAmount(1, numSegments/3);

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
        LFO<SawDown> lfo = LFO<SawDown>(5000);

    public:
        SegmentChasePattern(){
            this->name = "Segment chase";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;


            int segmentSize = 60;
            int numSegments = width / segmentSize;

            perm.setSize(numSegments);

            float pulseWidth = params->getSize(0.25,1);
            float factor = params->getAmount(50,1); //10; // 1-50;
            lfo.setPeriod(params->getVelocity(2000,200) * factor);
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

 class GlowPulsePattern : public Pattern<RGBA>
    {
        Permute perm;
        LFO<Glow> lfo = LFO<Glow>(10000);
        Transition transition;

    public:
        GlowPulsePattern(){
            this->name = "Glow pulse";
        }
        
        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            // float density = 481./width;
            int density2 = width / 481;
            if (width==96) density2=10;
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

}