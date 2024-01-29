#pragma once
#include "core/generation/patterns/pattern.hpp"
#include "generation/patterns/helpers/fade.h"
#include "generation/patterns/helpers/interval.h"
#include "generation/patterns/helpers/timeline.h"
#include "generation/pixelMap.hpp"
#include "mappingHelpers.hpp"
#include <math.h>
#include <vector>

namespace Patterns
{

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
                pixels[perm.at[index]] = params->getPrimaryColour() * transition.getValue();
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
                // RGBA color = params->getGradient(fromTop(map->z(index))*255);
                pixels[index] = color * lfo.getValue(-2 * around(map->th(index))) * Utils::rescale(map->r(index), 0.7, 1, 0.4, 1) * transition.getValue();
                // pixels[index] = color * lfo.getValue(fromTop(map->z(index))) * transition.getValue();
            }
        }
    };

    class RadialGlitterFadePattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap3d::Cylindrical *map;
        FadeDown fade1 = FadeDown(200);
        FadeDown fade2 = FadeDown(200);
        BeatWatcher watcher = BeatWatcher();
        Permute perm1;
        Permute perm2;
        int fadeNr = 0;

    public:
        RadialGlitterFadePattern(PixelMap3d::Cylindrical *map)
        {
            this->map = map;
            this->perm1 = Permute(map->size());
            this->perm2 = Permute(map->size());
            this->name = "Radial glitter fade";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            fade1.duration = params->getIntensity(1500, 300);
            fade2.duration = params->getIntensity(1500, 300);

            if (watcher.Triggered())
            {
                fadeNr = (fadeNr + 1) % 2;
                if (fadeNr == 0)
                {
                    fade1.reset();
                    perm1.permute();
                }
                else
                {
                    fade2.reset();
                    perm2.permute();
                }
            }

            float velocity = params->getVelocity(600, 100);

            if (perm1.at == 0 || perm2.at == 0)
                return;

            for (int i = 0; i < map->size(); i++)
            {

                float density = 4000. / width;
                fade1.duration = 100;
                fade2.duration = 100;
                if (perm1.at[i] < density * map->size() / 10)
                    fade1.duration *= perm1.at[i] * 4 / (density * map->size() / 10);
                if (perm2.at[i] < density * map->size() / 10)
                    fade2.duration *= perm2.at[i] * 4 / (density * map->size() / 10);

                float conePos = (0.5 + (map->r(i)) / 2);

                float fadePosition1 = fade1.getValue(conePos * velocity);
                RGBA color1 = params->getGradient(fadePosition1 * 255);
                pixels[i] = color1 * fadePosition1 * (1.5 - map->r(i)) * transition.getValue();

                float fadePosition2 = fade2.getValue(conePos * velocity);
                RGBA color2 = params->getGradient(fadePosition2 * 255);
                pixels[i] += color2 * fadePosition2 * (1.5 - map->r(i)) * transition.getValue();
            }
        }
    };

    class RadialFadePattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap3d::Cylindrical *map;
        FadeDown fade = FadeDown(200);
        BeatWatcher watcher = BeatWatcher();

    public:
        RadialFadePattern(PixelMap3d::Cylindrical *map)
        {
            this->map = map;
            this->name = "Radial fade";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            fade.duration = params->getSize(500, 120);
            int velocity = params->getVelocity(3000, 500);

            if (!transition.Calculate(active))
                return;

            if (watcher.Triggered())
            {
                fade.reset();
            }

            for (int i = 0; i < map->size(); i++)
            {
                float conePos = 0.20 + (map->r(i)) / 2;
                pixels[i] += params->getPrimaryColour() * fade.getValue(conePos * velocity) * transition.getValue();
            }
        }
    };

    class LineLaunch : public Pattern<RGBA>
    {
        FadeDown fade = FadeDown(50);
        BeatWatcher watcher = BeatWatcher();
        PixelMap3d *map;

    public:
        LineLaunch(PixelMap3d *map)
        {
            this->map = map;
            this->name = "Line launch";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active && fade.getPhase() == 1)
                return;

            int beatDiv = params->getIntensity(0, 4);
            if (beatDiv > 3)
                beatDiv = 3;
            int divs[] = {8, 4, 2, 1};

            if (watcher.Triggered() && Tempo::GetBeatNumber() % divs[beatDiv] == 0)
                fade.reset();

            float velocity = 100; // params->getVelocity(600, 100);

            for (int i = 0; i < width; i++)
            {
                float fadePosition = fade.getValue((0.3 + map->y(i)) * velocity);
                RGBA color = params->getSecondaryColour();
                pixels[i] += color * fadePosition;
            }
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

    class WindowGlitchPattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        Timeline timeline = Timeline(1000);
        int window = 0;
        const int numWindows = 8;

    public:
        WindowGlitchPattern()
        {
            this->name = "Window glitch";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            // i made a timeline longer than 60, and reset it here manually,
            // so it resets exactly during this frame instead of freely cycling at its own pace
            timeline.FrameStart();
            if (timeline.Happened(60))
            {
                timeline.reset();
                window = (window + 1 + Utils::random(0, numWindows - 1)) % numWindows;
            }

            int windowSize = width / numWindows;
            int startIndex = window * windowSize;

            RGBA col = params->getSecondaryColour() * transition.getValue();
            for (int j = 0; j < windowSize; j++)
                pixels[startIndex + j] += col;
        }
    };
}