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

namespace Patterns
{
    class SnakePattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<SawDown> lfo = LFO<SawDown>(2000);

    public:
        public:
        SnakePattern(){
            this->name = "Snake";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            lfo.setPeriod(5000 /*params->getVelocity(10000, 500)*/);
            lfo.setDutyCycle(0.4 /*params->getSize(0.33, 1)*/);
            int amount = 5; //params->getAmount() * 7 + 1;

            if (!transition.Calculate(active))
                return;

            for (int i = 0; i < LedsterShapes::snake.size(); i++)
            {
                float lfoVal = lfo.getValue((float)i / LedsterShapes::snake.size() * amount);
                pixels[LedsterShapes::snake[i]] += params->gradient->get(255 * lfoVal) * lfoVal * transition.getValue();
            }
        }
    };

    class XY : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap3d *map;
        LFO<Sin> lfoX = LFO<Sin>(2000);
        LFO<Sin> lfoZ = LFO<Sin>(2000);

    public:
        XY(PixelMap3d *map)
        {
            this->map = map;
            this->name="XY";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;
 
            auto col = params->getSecondaryColour() * transition.getValue();
            float size = 0.03; //params->getSize(0.01,0.03);
            lfoX.setPeriod(10000 /*params->getVelocity(4*20000,2000)*/);
            lfoZ.setPeriod(13000 /*params->getVelocity(4*14000,1400)*/);

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                if (map->y(index) < 0.44)
                    continue;

                float x_norm = (map->x(index)/0.7+1)/2;
                float z_norm = (map->z(index)/0.6+1)/2;

                float dim = std::max(
                    softEdge(abs(x_norm - lfoX.getValue()), size),
                    softEdge(abs(z_norm - lfoZ.getValue()), size)
                );

                pixels[index] = col * dim;
            }
        }
    };

    class PetalRotatePattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<SawDown> lfo = LFO<SawDown>(600);

    public:
        PetalRotatePattern(){
            this->name = "Petal Rotate";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(params->getVelocity(4000,400));
            float phase = 0; // params->getOffset(0,45);

            //auto col = params->getPrimaryColour() * transition.getValue();
            int petalIndex=0;
            for (auto petal : LedsterShapes::petals)
            {
                for (int j = 0; j < 45; j++)
                {
                    //auto col = params->gradient->get(255 * j / 45);
                    float lfoVal = lfo.getValue((((float)j) + phase * petalIndex/6)/45.);
                    auto col = params->gradient->get(255 * lfoVal);
                    pixels[petal[j]] += col * lfoVal * transition.getValue();
                }
                petalIndex++;
            }
        }
    };

    class Lighthouse : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<Glow> lfo;
        PixelMap3d::Cylindrical *map;

    public:
        Lighthouse(PixelMap3d::Cylindrical *map)
        {
            this->map = map;
            this->name = "Lighthouse";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(params->getVelocity(5000, 500));
            lfo.setDutyCycle(params->getSize(0.06, 0.5));

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                RGBA color = params->getSecondaryColour();
                // RGBA color = params->gradient->get(fromTop(map->z(index))*255);
                pixels[index] = color * lfo.getValue(-2 * around(map->th(index))) * Utils::rescale(map->r(index),0.5,1,0.4,1) * transition.getValue();
                // pixels[index] = color * lfo.getValue(fromTop(map->z(index))) * transition.getValue();
            }
        }
    };

    class SnowflakePatternLedster : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        SnowflakePatternLedster()
        {
            this->name = "Snowflake";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            auto col = params->getPrimaryColour() * transition.getValue();
            for (auto petal : LedsterShapes::petals)
            {
                for (int j = 0; j < 45; j++)
                {
                    float dist = 1 - abs((float)j - 9 + 5 - 45. / 2) / 10;
                    pixels[petal[j]] += col * dist;
                }
            }

            auto hex = LedsterShapes::hexagons[9];
            for (int j = 0; j < hex.size(); j++)
            {
                float dist = abs(float(j % (hex.size() / 6)) - (hex.size() / 12) - 0.5) / 4;
                pixels[hex[j]] += col * dist;
            }

            hex = LedsterShapes::hexagons[3];
            for (int j = 0; j < hex.size(); j++)
            {
                float dist = abs(float(j % (hex.size() / 6)) - (hex.size() / 12) - 0.5) / 2;
                pixels[hex[j]] += col * dist;
            }
        }
    };

    class PetalChase : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap3d::Cylindrical *map;
        LFO<SoftSawDown> lfo1;

    public:
        PetalChase(PixelMap3d::Cylindrical *map)
        {
            this->map = map;
            this->name = "Petal chase";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            int amount1 =7; //10; //7; //params->getAmount(1, 12);
            lfo1.setPeriod(params->getVelocity(5000, 500));
            lfo1.setDutyCycle(params->getSize(0.06, 0.5));
            lfo1.setSoftEdgeWidth(0.1);

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                if (map->r(index) < 0.35)
                    continue;

                float lfoVal1 = lfo1.getValue(-amount1 * around(map->th(index)));
                RGBA color1 = params->gradient->get(lfoVal1 * 255);

                pixels[index] = color1 * lfoVal1 * transition.getValue();
            }
        }
    };

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

            lfo.setDutyCycle(params->getAmount(0.1, 0.5));
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
            float factor = 10; //params->getAmount(50,1); //10; // 1-50;
            lfo.setPeriod(500 * factor/*params->getVelocity(2000,200) * factor*/);
            lfo.setDutyCycle(pulseWidth / factor);
            float lfoWidth = segmentSize * factor;

            for (int segment = 0; segment < numSegments; segment++)
            {
                int randomSegment = perm.at[segment];

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
            FadeDown(200),
            FadeDown(200),
            FadeDown(200),
            FadeDown(200),
            FadeDown(200),
            FadeDown(200)};
        std::vector<float> radii[6];
        BeatWatcher watcher = BeatWatcher();
        // PixelMap3d::Cylindrical *map;
        Permute perm;
        int pos = 0;

    public:
        GrowingCirclesPattern(PixelMap3d *map)
        {
            // this->map = map.toCylindricalXZ();
            for (int i = 0; i < 6; i++)
            {
                float xc = cos(float(i) / 6 * 2 * M_PI);
                float yc = 0.4;
                float zc = sin(float(i) / 6 * 2 * M_PI);
                std::transform(map->begin(), map->end(), std::back_inserter(radii[i]), [xc, yc, zc](PixelPosition3d pos) -> float
                               { return sqrt(pow(pos.x - xc, 2) + pow(pos.y - yc, 2) + pow(pos.z - zc, 2)); });
            }
            this->perm = Permute(map->size());
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

    class SpiralPattern : public Pattern<RGBA>
    {
        Transition transition = Transition();
        PixelMap3d::Cylindrical *map;
        LFO<SinFast> lfo;

    public:
        SpiralPattern(PixelMap3d::Cylindrical *map)
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
                float spiral = amount * (around(map->th(i)) + map->r(i) * curliness) + fromBottom(map->z(i));
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
                RGBA color = params->gradient->get(255 - 255 * map->r(i));

                pixels[i] = color * fadePosition * transition.getValue();
            }
        }
    };
}