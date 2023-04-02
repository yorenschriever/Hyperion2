#pragma once
#include "core/generation/patterns/helpers/fade.h"
#include "core/generation/patterns/helpers/interval.h"
#include "core/generation/patterns/helpers/timeline.h"
#include "core/generation/patterns/pattern.hpp"
#include "core/generation/pixelMap.hpp"
#include "platform/includes/utils.hpp"
#include <math.h>
#include <vector>

class RadialFadePattern : public Pattern<RGBA>
{
    Transition transition = Transition(
        200, Transition::none, 0,
        1000, Transition::none, 0);
    PixelMap map;
    FadeDown fade = FadeDown(200, WaitAtEnd);
    std::vector<float> normalizedRadii;
    BeatWatcher watcher = BeatWatcher();

public:
    RadialFadePattern(PixelMap map)
    {
        this->map = map;
        std::transform(map.begin(), map.end(), std::back_inserter(normalizedRadii), [](PixelPosition pos) -> float
                       { return sqrt(pos.y * pos.y + pos.x * pos.x); });
    }

    inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    {
        fade.duration = params->getIntensity(500, 120);
        int velocity = params->getVelocity(500, 50);

        if (!transition.Calculate(active))
            return;

        if (watcher.Triggered())
        {
            fade.reset();
        }

        for (int i = 0; i < normalizedRadii.size(); i++)
        {
            pixels[i] += params->getPrimaryColour() * fade.getValue(normalizedRadii[i] * velocity) * transition.getValue();
        }
    }
};

class ChevronsPattern : public Pattern<RGBA>
{
    PixelMap map;
    LFO<SawDown> lfo;
    LFO<Square> lfoColour;
    Transition transition = Transition(
        200, Transition::none, 0,
        1000, Transition::none, 0);
    FadeDown fade1 = FadeDown(1400, WaitAtEnd);

public:
    ChevronsPattern(PixelMap map)
    {
        this->map = map;
        this->lfo = LFO<SawDown>(1000);
        this->lfoColour = LFO<Square>(1000);
    }

    inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    {
        if (!transition.Calculate(active))
            return;

        float amount = params->getIntensity(0.25, 4);
        lfo.setPeriod(params->getVelocity(2000, 500));
        lfoColour.setPeriod(params->getVariant(2000, 500));

        for (int index = 0; index < std::min(width, (int)map.size()); index++)
        {
            float phase = (0.5 * abs(map[index].x) + map[index].y) * amount;
            auto col = lfoColour.getValue(phase) ? params->getSecondaryColour() : params->getPrimaryColour();
            pixels[index] += col * lfo.getValue(phase) * transition.getValue();
        }
    }
};

class RadialGlitterFadePattern : public Pattern<RGBA>
{
    Transition transition = Transition(
        200, Transition::none, 0,
        1000, Transition::none, 0);
    PixelMap map;
    FadeDown fade = FadeDown(200, WaitAtEnd);
    std::vector<float> normalizedRadii;
    // Timeline timeline = Timeline(1000);
    BeatWatcher watcher = BeatWatcher();
    Permute perm;

public:
    RadialGlitterFadePattern(PixelMap map)
    {
        this->map = map;
        std::transform(map.begin(), map.end(), std::back_inserter(normalizedRadii), [](PixelPosition pos) -> float
                       { return sqrt(pos.y * pos.y + pos.x * pos.x); });
        this->perm = Permute(map.size());
    }

    inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    {
        if (!transition.Calculate(active))
            return;

        fade.duration = params->getIntensity(500, 100);

        // timeline.FrameStart();
        // if (timeline.Happened(0))
        if (watcher.Triggered())
        {
            fade.reset();
            perm.permute();
        }

        float velocity = params->getVelocity(600, 100);
        float trail = params->getIntensity(1, 3);

        for (int i = 0; i < normalizedRadii.size(); i++)
        {
            fade.duration = perm.at[i] * trail; // + 100;
            pixels[i] += params->getSecondaryColour() * fade.getValue(normalizedRadii[i] * velocity);
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

    inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    {
        if (!transition.Calculate(active))
            return;

        timeline.FrameStart();
        perm.setSize(width);

        if (timeline.Happened(0))
            perm.permute();

        uint8_t val = timeline.GetTimelinePosition() < 25 ? 255 * transition.getValue() : 0;

        for (int index = 0; index < width / 30; index++)
            pixels[perm.at[index]] += params->getSecondaryColour() * val;
    }
};

template <class T>
class ClivePattern : public Pattern<RGBA>
{
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
        this->lfo.setPulseWidth(pulsewidth);
    }

    inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    {
        if (!active)
            return;

        for (int index = 0; index < width; index++)
        {
            int permutedQuantized = perm.at[index * numSegments / width] * width / numSegments;
            int interval = averagePeriod + permutedQuantized * (averagePeriod * precision) / width;
            pixels[index] += params->getSecondaryColour() * lfo.getValue(0, interval);
        }
    }
};

class RadialRainbowPattern : public Pattern<RGBA>
{
    Transition transition = Transition(
        200, Transition::none, 0,
        1000, Transition::none, 0);
    PixelMap map;
    std::vector<float> normalizedRadii;
    LFO<SawUp> lfo = LFO<SawUp>(1000);

public:
    RadialRainbowPattern(PixelMap map)
    {
        this->map = map;
        std::transform(map.begin(), map.end(), std::back_inserter(normalizedRadii), [](PixelPosition pos) -> float
                       { return sqrt(pos.y * pos.y + pos.x * pos.x); });
    }

    inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    {
        if (!transition.Calculate(active))
            return;

        lfo.setPeriod(params->getVelocity(4000, 500));
        float scale = params->getIntensity(0.5, 3);
        if (params->getVariant() < 0.5)
            scale *= -1;

        for (int i = 0; i < normalizedRadii.size(); i++)
        {
            pixels[i] += ((RGBA)Hue((normalizedRadii[i] * scale + lfo.getValue()) * 255)) * transition.getValue();
        }
    }
};

class StrobePattern : public Pattern<RGBA>
{
    int framecounter = 1;

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

class FlashesPattern : public Pattern<RGBA>
{
    FadeDown fade = FadeDown(2400, WaitAtEnd);
    BeatWatcher watcher = BeatWatcher();

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
            color = params->getSecondaryColour() + RGBA(255, 255, 255, 255) * ((val - 0.5) * 2);
        else
            color = params->getSecondaryColour() * ((val - 0.5) * 2);

        for (int index = 0; index < width; index++)
            pixels[index] = color;
    }
};



    class RadialGlitterFadePattern2 : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap::Polar map;
        FadeDown fade = FadeDown(200, WaitAtEnd);
        BeatWatcher watcher = BeatWatcher();
        Permute perm;

    public:
        RadialGlitterFadePattern2(PixelMap map)
        {
            this->map = map.toPolarRotate90();
            this->perm = Permute(map.size());
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return;

            fade.duration = params->getIntensity(500, 100);

            if (watcher.Triggered())
            {
                fade.reset();
                perm.permute();
            }

            float velocity = params->getVelocity(600, 100);

            for (int i = 0; i < map.size(); i++)
            {

                float density = 481. / width;
                fade.duration = 100; // trail + perm.at[i] / (density * map.size()/ 10);
                if (perm.at[i] < density * map.size() / 10)
                    fade.duration *= perm.at[i] * 4 / (density * map.size() / 10);

                float conePos = 0.5 + (map[i].r ) / 2;

                float fadePosition = fade.getValue(conePos * velocity);
                RGBA color = params->gradient->get(fadePosition * 255);
                pixels[i] = color * fadePosition * (1.5 - map[i].r) * transition.getValue();
            }
        }
    };

        class LineLaunch : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        FadeDown fade = FadeDown(50, WaitAtEnd);
        BeatWatcher watcher = BeatWatcher();
        PixelMap map;

    public:
        LineLaunch(PixelMap map)
        {
            this->map = map;
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!active && fade.getPhase() ==1)
                return;

            if (watcher.Triggered())
                fade.reset();

            float velocity = 100; //params->getVelocity(600, 100);
            // float density = 481./width;

            for (int i = 0; i < width; i++)
            {
                float fadePosition = fade.getValue((1+map[i].y) * velocity);
                RGBA color = params->getHighlightColour(); 
                pixels[i] += color * fadePosition; // * transition.getValue();
            }
        }
    };


    class SegmentChasePattern : public Pattern<RGBA>
    {
        Permute perm;
        LFO<LFOPause<SawDown>> lfo = LFO<LFOPause<SawDown>>(5000);

    public:
        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!active)
               return;

            int segmentSize =  60;
            int numSegments = width/segmentSize;

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
                    RGBA col = params->gradient->get(lfoVal * 255) * lfoVal;
                    pixels[segment * segmentSize + j] = col;
                }
            }


        }
    };