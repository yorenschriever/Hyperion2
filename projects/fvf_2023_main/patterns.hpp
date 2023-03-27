#pragma once
#include "core/generation/patterns/pattern.hpp"
#include "generation/patterns/helpers/fade.h"
#include "generation/patterns/helpers/interval.h"
#include "generation/patterns/helpers/timeline.h"
#include "generation/pixelMap.hpp"
#include "ledsterPatterns.hpp"
#include <math.h>
#include <vector>

namespace FWF
{

    class RadialGlitterFadePattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap::Polar map;
        FadeDown fade = FadeDown(200, WaitAtEnd);
        BeatWatcher watcher = BeatWatcher();
        Permute perm;

    public:
        RadialGlitterFadePattern(PixelMap::Polar map)
        {
            this->map = map;
            this->perm = Permute(map.size());
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            fade.duration = Params::getIntensity(500, 100);

            // timeline.FrameStart();
            // if (timeline.Happened(0))
            if (watcher.Triggered())
            {
                fade.reset();
                perm.permute();
            }

            float velocity = Params::getVelocity(600, 100);
            // float trail = Params::getIntensity(0,1) * density;
            //float trail = Params::getIntensity(0, 200);

            for (int i = 0; i < map.size(); i++)
            {
                // fade.duration = perm.at[i] * trail; // + 100;
                // fade.duration = (perm.at[i] * trail) < map.size() / 20 ? ; // + 100;

                // fade.duration = 100;
                // if (perm.at[i] < density * map.size()/ 10)
                //     fade.duration *= 4;

                float density = 481./width;
                fade.duration = 100; // trail + perm.at[i] / (density * map.size()/ 10);
                if (perm.at[i] < density * map.size() / 10)
                    fade.duration *= perm.at[i] * 4 / (density * map.size() / 10);

                float fadePosition = fade.getValue(map[i].r * velocity);
                RGBA color = Params::palette->get(fadePosition * 255);
                pixels[i] = color * fadePosition * (1.5 - map[i].r) * transition.getValue();
            }
        }
    };

    class PaletteGradientPattern : public Pattern<RGBA>
    {
        PixelMap::Polar map;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        PaletteGradientPattern(PixelMap::Polar map)
        {
            this->map = map;
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            for (int index = 0; index < std::min(width, (int)map.size()); index++)
            {
                RGBA colour = Params::palette->get(255 - map[index].r * 255);
                pixels[index] = colour * transition.getValue();
            }
        }
    };

    class AngularFadePattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap::Polar map;
        FadeDown fade = FadeDown(200, WaitAtEnd);
        BeatWatcher watcher = BeatWatcher();


    public:
        AngularFadePattern(PixelMap::Polar map)
        {
            this->map = map;
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            if (watcher.Triggered())
            {
                fade.reset();
                // perm.permute();
            }

            //float density = 481./width;
            float velocity = Params::getVelocity(200, 30);
            //float trail = Params::getIntensity(0, 200);

            for (int i = 0; i < map.size(); i++)
            {
                fade.duration = 100; // trail + perm.at[i] / (density * map.size()/ 10);
                // if (perm.at[i] < density * map.size()/ 10)
                //     fade.duration *= perm.at[i] * 4 / (density * map.size()/ 10);

                float fadePosition = fade.getValue(abs(map[i].th) * velocity);
                RGBA color = Params::palette->get(255 - abs(map[i].th) / M_PI * 255);
                pixels[i] = color * fadePosition * (map[i].r * 1.5) * transition.getValue();;
            }
        }
    };

    class GlowPulsePattern : public Pattern<RGBA>
    {
        Permute perm;
        LFO<LFOPause<NegativeCosFast>> lfo = LFO<LFOPause<NegativeCosFast>>(10000);
        Transition transition;

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            //float density = 481./width;
            int density2 = width/481;
            lfo.setPeriod(Params::getVelocity(10000,500));
            lfo.setPulseWidth(0.1);
            perm.setSize(width);

            for (int index = 0; index < width; index++)
            {
                if (index % density2 != 0)
                   continue;
                pixels[perm.at[index]] = Params::getHighlightColour() * lfo.getValue(float(index)/width) * transition.getValue(index, width);
            }
        }
    };

    class SquareGlitchPattern : public Pattern<RGBA>
    {
        //Timeline timeline = Timeline(50);
        Permute perm = Permute(256);
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

        PixelMap map;

    public:
        SquareGlitchPattern(PixelMap map)
        {
            this->map = map;
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            //timeline.FrameStart();

            //if (timeline.Happened(0))
                perm.permute();

            int threshold = Params::getIntensity(width * 0.01, width * 0.2);
            int numSquares = Params::getVariant(2,9);

            for (int index = 0; index < width; index++)
            {
                int xquantized = (map[index].x + 1) * numSquares;
                int yquantized = (map[index].y + 1) * numSquares;
                int square = xquantized + yquantized * numSquares;
                if (perm.at[square] > threshold / numSquares )
                    continue;
                pixels[index] += Params::getHighlightColour() * transition.getValue();
            }
        }
    };

    class GrowingStrobePattern : public Pattern<RGBA>
    {
        PixelMap::Polar map;
        FadeDown fade = FadeDown(5000, WaitAtEnd);

    public:
        GrowingStrobePattern(PixelMap::Polar map)
        {
            this->map = map;
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!active)
            {
                fade.reset();
                return;
            }

            RGBA col = Utils::millis() % 100 < 25 ? Params::getPrimaryColour() : RGBA();
            int directionUp = Params::getVariant() > 0.5;

            for (int i = 0; i < map.size(); i++)
            {
                if (directionUp && map[i].r < fade.getValue())
                    continue;
                if (!directionUp && 1.-map[i].r < fade.getValue())
                    continue;

                pixels[i] = col;                
            }
        }
    };


    template <class T>
    class RibbenClivePattern : public Pattern<RGBA>
    {
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
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!active)
                return;

            
            int numSegments = width/segmentSize;
            perm.setSize(numSegments);

            for (int ribbe = 0; ribbe < numSegments; ribbe++)
            {
                int interval = averagePeriod + perm.at[ribbe] * (averagePeriod * precision) / numSegments;
                RGBA col = Params::getPrimaryColour() * lfo.getValue(0, interval);
                for (int j = 0; j < segmentSize; j++)
                {
                    pixels[ribbe * segmentSize + j] = col;
                }
            }
        }
    };

    class RibbenFlashPattern : public Pattern<RGBA>
    {
        Permute perm;
        BeatWatcher watcher = BeatWatcher();
        FadeDown fade = FadeDown(2400, WaitAtEnd);

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!active)
                return;

            bool isLedster = width == 481;
            int segmentSize = isLedster ? 10 : 60;
            int numSegments = isLedster ? 36 : width/segmentSize;

            if (watcher.Triggered()) {
                perm.setSize(numSegments);
                perm.permute();
                fade.reset();
            }


            
            fade.duration = Params::getVelocity(2500,100);

            int numVisible = Params::getIntensity(1,numSegments);

            for (int ribbe = 0; ribbe < numVisible; ribbe++)
            {
                RGBA col = Params::getHighlightColour() * fade.getValue();
                if (isLedster)
                    for (int j = 0; j < segmentSize; j++)
                        pixels[Ledster::ribben[perm.at[ribbe]][j]] += col;
                else
                    for (int j = 0; j < segmentSize; j++)
                        pixels[perm.at[ribbe] * segmentSize + j] += col;
            }
        }
    };

    class RibbenFadePattern : public Pattern<RGBA>
    {
        Permute perm;
        LFO<LFOPause<SawDown>> lfo = LFO<LFOPause<SawDown>>(1000);

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!active)
               return;

            bool isLedster = width == 481;
            int segmentSize = isLedster ? 10 : 60;
            int numSegments = isLedster ? 36 : width/segmentSize;

            perm.setSize(numSegments);
            lfo.setPulseWidth(0.15);

            for (int segment = 0; segment < numSegments; segment++)
            {
                int randomSegment = perm.at[segment];
                RGBA col = Params::getPrimaryColour() * lfo.getValue(float(randomSegment)/numSegments);
                for (int j = 0; j < segmentSize; j++)
                    if (isLedster)
                        pixels[Ledster::ribben[segment][j]] += col;
                    else
                        pixels[segment * segmentSize + j] = col;
            }
        }
    };

    class SegmentChasePatternWITHFADE : public Pattern<RGBA>
    {
        Permute perm;
        //LFO<LFOPause<SawDown>> lfo = LFO<LFOPause<SawDown>>(5000);
        //LFO<PWM> segmentOnLfo = LFO<PWM>(200000);
        std::vector<FadeDown> fades;


    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!active)
               return;

            bool isLedster = width == 481;
            int segmentSize = isLedster ? 10 : 60;
            int numSegments = isLedster ? 36 : width/segmentSize;\
            float density = 481./width;

            if (fades.size() ==0){
                for (int segment = 0; segment < numSegments; segment++){
                    fades.push_back(FadeDown(1000,WaitAtEnd));
                }
            }
            perm.setSize(numSegments);

            // float amount = 0;
            // float lfoWidth = segmentSize + amount * width;
            // lfo.setPeriod(1000 + 1000*amount*numSegments);
            // float pulseWidth = 1;
            // lfo.setPulseWidth(pulseWidth / (1+amount * numSegments));

            if (Utils::random_f() < 0.5){
                fades[Utils::random(0,fades.size())].reset();
            }

            // lfo.setPulseWidth(1);

            for (int segment = 0; segment < numSegments; segment++)
            {
                FadeDown fade = fades[segment];
                //int randomSegment = perm.at[segment];
                
                for (int j = 0; j < segmentSize; j++) {
                    RGBA col = Params::getPrimaryColour() * fade.getValue(float(j)*10);
                    if (isLedster)
                        pixels[Ledster::ribben[segment][j]] += col;
                    else
                        pixels[segment * segmentSize + j] = col;
                }
            }


        }
    };

    class SegmentChasePattern : public Pattern<RGBA>
    {
        Permute perm;
        LFO<LFOPause<SawDown>> lfo = LFO<LFOPause<SawDown>>(5000);
        //LFO<PWM> segmentOnLfo = LFO<PWM>(200000);
        //std::vector<FadeDown> fades;


    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!active)
               return;

            bool isLedster = width == 481;
            int segmentSize = isLedster ? 10 : 60;
            int numSegments = isLedster ? 36 : width/segmentSize;

            perm.setSize(numSegments);

            float pulseWidth = 0.5;
            float factor = 10; //1-50;
            lfo.setPeriod(500 * factor);
            lfo.setPulseWidth(pulseWidth /factor);
            float lfoWidth = segmentSize * factor;



            for (int segment = 0; segment < numSegments; segment++)
            {
                int randomSegment = perm.at[segment];
                int direction = segment % 2 == 0;
                //todo direction
                
                for (int j = 0; j < segmentSize; j++) {
                    float lfoVal = lfo.getValue(float(j)/lfoWidth + float(randomSegment)/numSegments+float(segment));
                    RGBA col = Params::palette->get(lfoVal * 255) * lfoVal;
                    if (isLedster)
                        pixels[Ledster::ribben[segment][j]] += col;
                    else
                        pixels[segment * segmentSize + j] = col;
                }
            }


        }
    };

    class OnBeatColumnFadePattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        FadeDown fade[6] = {
            FadeDown(1400, WaitAtEnd),
            FadeDown(1400, WaitAtEnd),
            FadeDown(1400, WaitAtEnd),
            FadeDown(1400, WaitAtEnd),
            FadeDown(1400, WaitAtEnd),
            FadeDown(1400, WaitAtEnd)};
        BeatWatcher watcher = BeatWatcher();
        int pos = 0;

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            for(int i=0;i<6;i++) 
                fade[i].duration = Params::getIntensity(3000, 100);

            if (!transition.Calculate(active))
                return;

            if (watcher.Triggered())
            {
                pos = (pos + 1) % 6;
                fade[pos].reset();
            }

            for (int column = 0; column < 6; column++)
            {
                int columnStart = column * width / 6;
                int columnEnd = columnStart + width / 6;
                RGBA col = Params::getPrimaryColour() * fade[column].getValue() * transition.getValue();
                for (int j = columnStart; j < columnEnd; j++)
                    pixels[j] = col;
            }
        }
    };

};


