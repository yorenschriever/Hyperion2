#pragma once
#include "core/generation/patterns/helpers/fade.h"
#include "core/generation/patterns/helpers/interval.h"
#include "core/generation/patterns/helpers/timeline.h"
#include "core/generation/patterns/pattern.hpp"
#include "core/generation/pixelMap.hpp"
#include "platform/includes/utils.hpp"
#include <math.h>
#include <vector>
#include "utils.hpp"

//helper functions to map the bottom (z=-0.45) - top (z=0.45) to range 0-1
float fromTop(float y){
    return (y + 0.45)/0.9;
}

float fromBottom(float y){
    return (0.45 - y)/0.9;
}

float around(float th){
    return (th + M_PI)/(2*M_PI);
}

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
        this->lfo.setPulseWidth(pulsewidth);
    }

    inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    {
                if (!transition.Calculate(active))
            return;

        for (int index = 0; index < width; index++)
        {
            int permutedQuantized = perm.at[index * numSegments / width] * width / numSegments;
            int interval = averagePeriod + permutedQuantized * (averagePeriod * precision) / width;
            pixels[index] += params->getSecondaryColour() * lfo.getValue(0, interval) * transition.getValue();
        }
    }
};

class RadialRainbowPattern : public Pattern<RGBA>
{
    Transition transition = Transition(
        200, Transition::none, 0,
        1000, Transition::none, 0);
    PixelMap::Polar map;
    LFO<SawUp> lfo = LFO<SawUp>(1000);

public:
    RadialRainbowPattern(PixelMap::Polar map)
    {
        this->map = map;
    }

    inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    {
        if (!transition.Calculate(active))
            return;

        lfo.setPeriod(params->getVelocity(4000, 500));
        float scale = params->getSize(0.05, 3);
        if (params->getVariant() < 0.5)
            scale *= -1;
        float offset = params->getOffset();

        for (int i = 0; i < map.size(); i++)
        {
            float phase = map[i].r * scale + lfo.getValue() + offset * abs(map[i].th);
            Hue hue = Hue(int((phase) * 255 + 255)%255);
            pixels[i] = RGBA(hue) * transition.getValue();
        }
    }
    
};

class RadialGradientPattern : public Pattern<RGBA>
{
    Transition transition = Transition(
        200, Transition::none, 0,
        1000, Transition::none, 0);
    PixelMap::Polar map;
    LFO<SawUp> lfo = LFO<SawUp>(1000);

public:
    RadialGradientPattern(PixelMap::Polar map)
    {
        this->map = map;
    }

    inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    {
        if (!transition.Calculate(active))
            return;

        lfo.setPeriod(params->getVelocity(4000, 500));
        float scale = params->getSize(0.05, 3);
        if (params->getVariant() < 0.5)
            scale *= -1;
        float offset = params->getOffset();

        for (int i = 0; i < map.size(); i++)
        {
            float phase = map[i].r * scale + lfo.getValue() + offset * abs(map[i].th);
            RGBA color = params->gradient->get(int((phase) * 255 + 255)%255);
            pixels[i] = color * transition.getValue();
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

    inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
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

            float conePos = 0.5 + (map[i].r) / 2;

            float fadePosition = fade.getValue(conePos * velocity);
            RGBA color = params->gradient->get(fadePosition * 255);
            pixels[i] = color * fadePosition * (1.5 - map[i].r) * transition.getValue();
        }
    }
};

class LineLaunch : public Pattern<RGBA>
{
    FadeDown fade = FadeDown(50, WaitAtEnd);
    BeatWatcher watcher = BeatWatcher();
    PixelMap map;

public:
    LineLaunch(PixelMap map)
    {
        this->map = map;
    }

    inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    {
        if (!active && fade.getPhase() == 1)
            return;

        if (watcher.Triggered())
            fade.reset();

        float velocity = params->getVelocity(600, 100);
        fade.duration = params->getSize(250,20);
        RGBA color = params->getHighlightColour();

        for (int i = 0; i < width; i++)
            pixels[i] += color * fade.getValue((0.8 - map[i].y) * velocity);
    }
};

class SegmentChasePattern : public Pattern<RGBA>
{
        Transition transition = Transition(
        200, Transition::none, 0,
        1000, Transition::none, 0);
    Permute perm;
    LFO<LFOPause<SawDown>> lfo = LFO<LFOPause<SawDown>>(5000);

public:
    inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    {
        if (!transition.Calculate(active))
            return;

        int segmentSize = 60;
        int numSegments = width / segmentSize;

        perm.setSize(numSegments);

        float pulseWidth = 0.5;
        float factor = 10; // 1-50;
        lfo.setPeriod(500 * factor);
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
                RGBA col = params->gradient->get(lfoVal * 255) * lfoVal;
                pixels[segment * segmentSize + j] = col * transition.getValue();
            }
        }
    }
};

class HorizontalGradientPattern : public Pattern<RGBA>
{
    PixelMap map;
    Transition transition = Transition(
        200, Transition::none, 0,
        1000, Transition::none, 0);

public:
    HorizontalGradientPattern(PixelMap map)
    {
        this->map = map;
    }

    inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    {
        if (!transition.Calculate(active))
            return;

        for (int index = 0; index < std::min(width, (int)map.size()); index++)
        {
            // float phase = (map[index].x + 1) / 2.;
            // RGBA colour = params->getSecondaryColour() + params->getPrimaryColour() * phase;
            RGBA colour = params->gradient->get(abs(map[index].x) * 255);
            RGBA dimmedColour = colour * transition.getValue();
            pixels[index] += dimmedColour;
        }
    }
};

class PaletteTester : public Pattern<RGBA>
{
    PixelMap map;

public:
    PaletteTester(PixelMap map)
    {
        this->map = map;
    }

    inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    {
        if (!active) return;

        for (int index = 0; index < std::min(width, (int)map.size()); index++)
        {
            if (map[index].y > 0.7) 
                pixels[index] = params->getHighlightColour();
            else if (map[index].y > 0.6) 
                pixels[index] = params->getSecondaryColour();
            else if (map[index].y > 0.5) 
                pixels[index] = params->getPrimaryColour();
            else
                pixels[index] = params->gradient->get(Utils::rescale(map[index].x,0,255,-1,1));
        }
    }
};



    class Lighthouse : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<LFOPause<NegativeCosFast>> lfo;
        PixelMap::Polar map;

    public:
        Lighthouse(PixelMap::Polar map)
        {
            this->map = map;
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(params->getVelocity(5000, 500));
            lfo.setPulseWidth(params->getSize(0.06, 0.5));

            for (int index = 0; index < std::min(width, (int)map.size()); index++)
            {
                RGBA color = params->getSecondaryColour();
                // RGBA color = params->gradient->get(fromTop(map[index].z)*255);
                pixels[index] = color * lfo.getValue(-2 * around(map[index].th)) * transition.getValue();
                // pixels[index] = color * lfo.getValue(fromTop(map[index].z)) * transition.getValue();
            }
        }
    };


        class XY : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap map;
        LFO<Sin> lfoX = LFO<Sin>(2000);
        LFO<Sin> lfoZ = LFO<Sin>(2000);

    public:
        XY(PixelMap map)
        {
            this->map = map;
        }

        inline float softEdge(float dist, float size)
        {
            float edgeSize = 0.03;
            if (dist > size) return 0;
            
            if (dist < size - edgeSize) return 1;
            
            return (size-dist) / edgeSize;
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            
            auto col = params->getSecondaryColour() * transition.getValue();
            float size = params->getSize(0.02,0.1);
            lfoX.setPeriod(params->getVelocity(4*20000,2000));
            lfoZ.setPeriod(params->getVelocity(4*14000,1400));
            int variant = params->getVariant(0,3);

            for (int index = 0; index < std::min(width, (int)map.size()); index++)
            {
                float x_norm = Utils::rescale(map[index].x,0,1,-1.2,1.2);
                float z_norm = Utils::rescale(map[index].y,0,1,0,1);

                if (variant==1) x_norm = 99;
                if (variant==2) z_norm = 99;

                float dim = std::max(
                    softEdge(abs(x_norm - lfoX.getValue()), size),
                    softEdge(abs(z_norm - lfoZ.getValue()), size)
                );

                pixels[index] = col * dim;
            }
        }
    };


    //     class LineLaunch : public Pattern<RGBA>
    // {
    //     Transition transition = Transition(
    //         200, Transition::none, 0,
    //         1000, Transition::none, 0);
    //     FadeDown fade = FadeDown(50, WaitAtEnd);
    //     BeatWatcher watcher = BeatWatcher();
    //     PixelMap map;

    // public:
    //     LineLaunch(PixelMap map)
    //     {
    //         this->map = map;
    //     }

    //     inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    //     {
    //         if (!active && fade.getPhase() == 1)
    //             return;

    //         if (watcher.Triggered())
    //             fade.reset();

    //         float velocity = 100; // params->getVelocity(600, 100);
    //         // float density = 481./width;

    //         for (int i = 0; i < width; i++)
    //         {
    //             float fadePosition = fade.getValue((1 + map[i].y) * velocity);
    //             RGBA color = params->getHighlightColour();
    //             pixels[i] += color * fadePosition; // * transition.getValue();
    //         }
    //     }
    // };


    class GrowingStrobePattern : public Pattern<RGBA>
    {
        PixelMap::Polar map;
        FadeDown fade = FadeDown(2000, WaitAtEnd);

    public:
        GrowingStrobePattern(PixelMap::Polar map)
        {
            this->map = map;
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
            {
                fade.reset();
                return;
            }

            RGBA col = Utils::millis() % 100 < 25 ? params->getPrimaryColour() : RGBA();
            int directionUp = params->getVariant() > 0.5;

            for (int i = 0; i < map.size(); i++)
            {
                float conePos = Utils::rescale(map[i].r,0,1,0.4,1.5);
                if (directionUp && conePos < fade.getValue())
                    continue;
                if (!directionUp && 1. - conePos < fade.getValue())
                    continue;

                pixels[i] = col;
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

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            if (watcher.Triggered())
            {
                fade.reset();
                // perm.permute();
            }

            // float density = 481./width;
            float velocity = params->getVelocity(500, 100);
            // float trail = params->getIntensity(0, 200);

            for (int i = 0; i < map.size(); i++)
            {
                fade.duration = 80; // trail + perm.at[i] / (density * map.size()/ 10);
                // if (perm.at[i] < density * map.size()/ 10)
                //     fade.duration *= perm.at[i] * 4 / (density * map.size()/ 10);
                float th = M_PI-abs(map[i].th);

                float fadePosition = fade.getValue(th * velocity);
                RGBA color = params->gradient->get(255 - th / M_PI * 255);
                pixels[i] = color * fadePosition * (map[i].r * 1.5) * transition.getValue();
                ;
            }
        }
    };


        class GlowPulsePattern : public Pattern<RGBA>
    {
        Permute perm;
        LFO<LFOPause<NegativeCosFast>> lfo = LFO<LFOPause<NegativeCosFast>>(10000);
        Transition transition;

    public:
        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            // float density = 481./width;
            int density2 = width / 481;
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
        Permute perm;
        int pos = 0;

    public:
        GrowingCirclesPattern(PixelMap map)
        {
            // this->map = map.toCylindricalRotate90();
            for (int i = 0; i < 6; i++)
            {
                float xc = cos(float(i) / 6 * 2 * M_PI);
                float yc = sin(float(i) / 6 * 2 * M_PI);
                std::transform(map.begin(), map.end(), std::back_inserter(radii[i]), [xc, yc](PixelPosition pos) -> float
                               { return sqrt(pow(pos.x - xc, 2) + pow(pos.y - yc, 2)); });
            }
            this->perm = Permute(map.size());
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

            float velocity = params->getVelocity(1000, 200);
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


    template <class T>
    class RibbenClivePattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
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
            // lfo.setPeriod(params->getVelocity(10000,500));

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