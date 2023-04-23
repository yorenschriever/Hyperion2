#pragma once
#include "core/generation/patterns/pattern.hpp"
#include "generation/patterns/helpers/fade.h"
#include "generation/patterns/helpers/interval.h"
#include "generation/patterns/helpers/timeline.h"
#include "generation/pixelMap.hpp"
#include "ledsterShapes.hpp"
#include "mappingHelpers.hpp"
#include <math.h>
#include <vector>

namespace Min
{

    template <class T>
    class RibbenClivePattern : public Pattern<RGBA>
    {
        Transition transition;
        const int segmentSize = 60;
        int averagePeriod;
        float precision;
        LFO<T> lfo = LFO<T>();
        Permute perm;

    public:
        RibbenClivePattern(int averagePeriod = 10000, float precision = 1, float pulsewidth = 0.025)
        {
            this->averagePeriod = averagePeriod;
            this->precision = precision;
            this->lfo.setPulseWidth(pulsewidth);
            this->name = "Ribben clive";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            bool isLedster = width == 481;
            int segmentSize = isLedster ? 10 : 60;
            int numSegments = isLedster ? 36 : width / segmentSize;

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

            lfo.setPulseWidth(params->getAmount(0.1, 1));
            //lfo.setPeriod(params->getVelocity(10000,500));

            for (int segment = 0; segment < numSegments; segment++)
            {
                // int randomSegment = perm.at[segment];
                // RGBA col = params->getPrimaryColour() * lfo.getValue(float(randomSegment)/numSegments);
                int interval = averagePeriod + perm.at[segment] * (averagePeriod * precision) / numSegments;
                RGBA col = params->getPrimaryColour() * lfo.getValue(0, interval) * transition.getValue();
                for (int j = 0; j < segmentSize; j++)
                    if (isLedster)
                        pixels[LedsterShapes::ribben[segment][j]] += col;
                    else
                        pixels[segment * segmentSize + j] = col;
            }
        }
    };

    class RibbenFlashPattern : public Pattern<RGBA>
    {
        Transition transition;
        Permute perm;
        BeatWatcher watcher = BeatWatcher();
        FadeDown fade = FadeDown(2400, WaitAtEnd);

    public:
        RibbenFlashPattern() {
            this->name = "Ribben flash";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            bool isLedster = width == 481;
            int segmentSize = isLedster ? 10 : 60;
            int numSegments = isLedster ? 36 : width / segmentSize;
            perm.setSize(numSegments);

            if (watcher.Triggered())
            {

                perm.permute();
                fade.reset();
            }

            fade.duration = params->getVelocity(2500, 100);
            int numVisible = params->getAmount(1, numSegments);

            for (int ribbe = 0; ribbe < numVisible; ribbe++)
            {
                RGBA col = params->getPrimaryColour() * fade.getValue() * transition.getValue();
                if (isLedster)
                    for (int j = 0; j < segmentSize; j++)
                        pixels[LedsterShapes::ribben[perm.at[ribbe]][j]] += col;
                else
                    for (int j = 0; j < segmentSize; j++)
                        pixels[perm.at[ribbe] * segmentSize + j] += col;
            }
        }
    };

    class SegmentChasePattern : public Pattern<RGBA>
    {
        Transition transition;
        Permute perm;
        LFO<LFOPause<SawDown>> lfo = LFO<LFOPause<SawDown>>(5000);

    public:
        SegmentChasePattern(){
            this->name = "Segment chase";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            bool isLedster = width == 481;
            bool isHalo = width==96;
            int segmentSize = isLedster ? 10 : 60;
            int numSegments = isLedster ? 36 : width / segmentSize;

            if (isHalo){
                segmentSize = width;
                numSegments = 1;
            }

            perm.setSize(numSegments);

            float pulseWidth = params->getSize(0.25,1);
            float factor = params->getAmount(50,1); //10; // 1-50;
            lfo.setPeriod(params->getVelocity(2000,200) * factor);
            lfo.setPulseWidth(pulseWidth / factor);
            float lfoWidth = segmentSize * factor;

            for (int segment = 0; segment < numSegments; segment++)
            {
                int randomSegment = perm.at[segment];
                int direction = segment % 2 == 0;
                // todo direction

                for (int j = 0; j < segmentSize; j++)
                {
                    float lfoVal = lfo.getValue(float(j) / lfoWidth + float(randomSegment) / numSegments + float(segment));
                    RGBA col = params->gradient->get(lfoVal * 255) * lfoVal * transition.getValue();
                    if (isLedster)
                        pixels[LedsterShapes::ribben[segment][j]] += col;
                    else
                        pixels[segment * segmentSize + j] = col;
                }
            }
        }
    };

    class GrowingCirclesPattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        FadeDown fade[6] = {
            FadeDown(200, WaitAtEnd),
            FadeDown(200, WaitAtEnd),
            FadeDown(200, WaitAtEnd),
            FadeDown(200, WaitAtEnd),
            FadeDown(200, WaitAtEnd),
            FadeDown(200, WaitAtEnd)};
        std::vector<float> radii[6];
        BeatWatcher watcher = BeatWatcher();
        // PixelMap3d::Cylindrical map;
        Permute perm;
        int pos = 0;

    public:
        GrowingCirclesPattern(PixelMap3d map)
        {
            // this->map = map.toCylindricalRotate90();
            for (int i = 0; i < 6; i++)
            {
                float xc = cos(float(i) / 6 * 2 * M_PI);
                float yc = 0.4;
                float zc = sin(float(i) / 6 * 2 * M_PI);
                std::transform(map.begin(), map.end(), std::back_inserter(radii[i]), [xc, yc, zc](PixelPosition3d pos) -> float
                               { return sqrt(pow(pos.x - xc, 2) + pow(pos.y - yc, 2) + pow(pos.z - zc, 2)); });
            }
            this->perm = Permute(map.size());
            this->name = "Growing circles";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            if (watcher.Triggered() && Tempo::GetBeatNumber() % 4 == 0)
            {
                pos = (pos + 1) % 6;
                fade[pos].reset();
            }

            float velocity = params->getVelocity(1000, 100);
            for(int i=0;i<6;i++)
                fade[i].duration = params->getSize(400,50)*velocity/1000;
            
            // float density = 481./width;

            for (int i = 0; i < width; i++)
            {
                for (int column = 0; column < 6; column++)
                {
                    float fadePosition = fade[column].getValue(radii[column][i] * velocity);
                    RGBA color = params->getPrimaryColour();
                    pixels[i] += color * fadePosition * transition.getValue();
                }
            }
        }
    };

    class LineLaunch : public Pattern<RGBA>
    {
        FadeDown fade = FadeDown(50, WaitAtEnd);
        BeatWatcher watcher = BeatWatcher();
        PixelMap3d map;

    public:
        LineLaunch(PixelMap3d map)
        {
            this->map = map;
            this->name = "Line launch";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active && fade.getPhase() == 1)
                return;

            if (watcher.Triggered())
                fade.reset();

            float velocity = 100; // params->getVelocity(600, 100);

            for (int i = 0; i < width; i++)
            {
                float fadePosition = fade.getValue((1 + map[i].z) * velocity);
                RGBA color = params->getHighlightColour();
                pixels[i] += color * fadePosition;
            }
        }
    };

    class GlowPulsePattern : public Pattern<RGBA>
    {
        Permute perm;
        LFO<LFOPause<NegativeCosFast>> lfo = LFO<LFOPause<NegativeCosFast>>(10000);
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
            lfo.setPulseWidth(0.1);
            perm.setSize(width);

            for (int index = 0; index < width; index++)
            {
                if (index % density2 != 0)
                    continue;
                pixels[perm.at[index]] = params->getHighlightColour() * lfo.getValue(float(index) / width) * transition.getValue(index, width);
            }
        }
    };

    class SpiralPattern : public Pattern<RGBA>
    {
        Transition transition = Transition();
        PixelMap3d::Cylindrical map;
        LFO<SinFast> lfo;

    public:
        SpiralPattern(PixelMap3d::Cylindrical map)
        {
            this->map = map;
            this->name = "Spiral";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(params->getVelocity(5000,300));
            int amount = params->getAmount(1,3);
            float curliness = params->getVariant(0.5,3);
            float size = params->getSize(0.01,0.1) * amount + curliness/24;
            
            for (int i = 0; i < width; i++)
            {
                float spiral = amount * (around(map[i].th) + map[i].r * curliness) + fromBottom(map[i].z);
                //while (spiral < 0) spiral += 1;
                //while (spiral > 1) spiral -= 1;
                float pos = abs(spiral - lfo.getPhase());

                //for some reason pos can become very large. so large that  subtracting 1 
                //fall withing the rounding accuracy, and the loop below becomes an infinite loop.
                //pos is never expected to be bigger than only a few units, so if we some something else,
                //i just ignore this frame.
                if (pos > 10) return;
                while(pos > 1) {
                    pos -= 1;
                }
                float fadePosition = softEdge(pos, size, 0.06);
                RGBA color = params->gradient->get(255 - 255 * map[i].r);

                pixels[i] = color * fadePosition * transition.getValue();
            }
        }
    };
}