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
    class StaticGradientPattern : public Pattern<RGBA>
    {
        PixelMap3d::Cylindrical *map;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        StaticGradientPattern(PixelMap3d::Cylindrical *map)
        {
            this->map = map;
            this->name = "Static gradient";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            float height = params->getSize(0.2, 0.75);

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                float h = map->r(index) * height;
                RGBA colour = params->getGradient(h * 255);
                pixels[index] = colour * h * transition.getValue();
            }
        }
    };

    class BreathingGradientPattern : public Pattern<RGBA>
    {
        PixelMap3d::Cylindrical *map;
        LFO<Glow> lfo;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        BreathingGradientPattern(PixelMap3d::Cylindrical *map)
        {
            this->map = map;
            this->name = "Breathing gradient";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            // float height = params->getSize(0.2,0.75);
            lfo.setPeriod(params->getVelocity(15000, 2000));
            float height = 0.25 + lfo.getValue() * params->getSize(0, 0.50);

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                float h = map->r(index) * height;
                RGBA colour = params->getGradient(h * 255);
                pixels[index] = colour * h * transition.getValue();
            }
        }
    };

    class BarLFO : public Pattern<RGBA>
    {
        int segmentSize;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<Glow> lfo;

    public:
        BarLFO(int segmentSize = 60)
        {
            this->segmentSize = segmentSize;
            this->name = "Bar LFO";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            int amount = 2*int(params->getAmount(1,5));
            lfo.setDutyCycle(params->getSize(0.25,1));
            lfo.setPeriod(params->getVelocity(1000,50)*amount);
            lfo.setSoftEdgeWidth(2*width/segmentSize);

            for (int bar = 0; bar < width; bar += segmentSize)
            {
                for (int i = 0; i < segmentSize; i++)
                    pixels[bar + i] = params->getSecondaryColour() * lfo.getValue(amount* float(bar) / width) * transition.getValue();
            }
        }
    };


class GradientLFO : public Pattern<RGBA>
    {
        int segmentSize;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        BeatWatcher wachter;
        LFO<SawDown> lfo;

    public:
        GradientLFO(int segmentSize = 60)
        {
            this->segmentSize = segmentSize;
            this->name = "Gradient LFO";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            float trailSize = params->getAmount(30, 120);
            lfo.setPeriod(params->getVelocity(4000,500));
            lfo.setDutyCycle(params->getSize());

            for (int bar = 0; bar < width / segmentSize; bar++)
            {
                for (int i = 0; i < segmentSize; i++)
                {
                    int center = segmentSize / 2;
                    int distance = abs(center - i);

                    float fadeValue = lfo.getValue(float(bar) / (width/ segmentSize) * 2 + distance / trailSize);
                    pixels[bar * segmentSize + i] = params->getGradientf(fadeValue) * fadeValue * transition.getValue();
                }
            }
        }
    };


    class FadeFromRandomChandelier : public Pattern<RGBA>
    {
        int segmentSize;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        FadeDown fade;
        BeatWatcher watcher;
        int beatDivs[6] = {16, 8, 4, 2, 1, 1};
        int centers[48];
        int delays[48];
        FadeDown masterFade;

    public:
        FadeFromRandomChandelier(int segmentSize = 60)
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
                for (int i = 0; i < 48; i++)
                {
                    centers[i] = Utils::random(15, 45);
                    delays[i] = Utils::random(0, 500);
                }
            }

            if (!transition.Calculate(active))
                return;

            masterFade.duration = Tempo::TimeBetweenBeats() * beatDiv;

            float trailSize = params->getVelocity(15, 5);
            fade.setDuration(params->getSize(1200, 300));
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

class VerticallyIsolated : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::fromStart, 1200,
            500, Transition::fromEnd, 1500);
        PixelMap3d *map;

    public:
        VerticallyIsolated(PixelMap3d *map)
        {
            this->map = map;
            this->name = "Vertically isolated";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            for (int index = 0; index < width; index++)
            {
                int z255 = fromBottom(map->y(index)) * 255;
                pixels[index] = params->getGradient(z255) * transition.getValue(255 - z255, 255);
            }
        }
    };

    class RotatingRingsPattern : public Pattern<RGBA>
    {
        Transition transition;
        PixelMap3d *map;
        LFO<Sin> ring1;
        LFO<Sin> ring2;

    public:
        RotatingRingsPattern(PixelMap3d *map)
        {
            this->map = map;
            this->name = "Rotating rings";
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

                pixels[index] = col1 * softEdge(abs(z_norm - ring1.getValue(offset)), size);
                pixels[index] += col2 * softEdge(abs(z_norm - ring2.getValue(offset)), size);
            }
        }
    };

    class OnBeatWindowChaseUpPattern : public Pattern<RGBA>
    {
        static const int numWindows = 8;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        FadeDown fade[numWindows] = {
            FadeDown(200),
            FadeDown(200),
            FadeDown(200),
            FadeDown(200),
            FadeDown(200),
            FadeDown(200),
            FadeDown(200),
            FadeDown(200)
        };
        BeatWatcher watcher = BeatWatcher();
        PixelMap3d *map;

        int pos = 0;

    public:
        OnBeatWindowChaseUpPattern(PixelMap3d *map)
        {
            this->map = map;
            // this->perm = Permute(map->size());
            this->name = "On beat window chase up";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            if (watcher.Triggered())
            {
                pos = (pos + 1) % numWindows;
                fade[pos].reset();
            }

            float velocity = params->getVelocity(2000, 100);
            float tailSize = params->getSize(300, 50);

            for (int column = 0; column < numWindows; column++)
            {
                int columnStart = column * width / numWindows;
                int columnEnd = columnStart + width / numWindows;

                fade[column].duration = tailSize;

                for (int i = columnStart; i < columnEnd; i++)
                {
                    float y = fromBottom(map->y(i));
                    float fadePosition = fade[column].getValue(y * velocity);
                    RGBA color = params->getPrimaryColour();
                    pixels[i] = color * fadePosition * (1 - y) * transition.getValue();
                }
            }
        }
    };

    class HorizontalSin : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<Glow> lfo;
        PixelMap3d *map;

    public:
        HorizontalSin(PixelMap3d *map)
        {
            this->map = map;
            this->name = "Horizontal sin";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(params->getVelocity(11000, 500));
            lfo.setDutyCycle(params->getSize(0.03, 0.5));

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                // RGBA color = params->getPrimaryColour();
                RGBA color = params->getGradient(fromBottom(map->y(index)) * 255);
                pixels[index] = color * lfo.getValue(normalize(map->z(index))) * transition.getValue();
            }
        }
    };

    class VerticalSin : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<Glow> lfo;
        PixelMap3d *map;

    public:
        VerticalSin(PixelMap3d *map)
        {
            this->map = map;
            this->name = "Vertical sin";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(params->getVelocity(11000, 500));
            lfo.setDutyCycle(params->getSize(0.03, 0.5));

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                // RGBA color = params->getPrimaryColour();
                RGBA color = params->getGradient(fromBottom(map->y(index)) * 255);
                pixels[index] = color * lfo.getValue(normalize(map->y(index))) * transition.getValue();
            }
        }
    };


    class GrowShrink : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<NegativeCosFast> lfo;
        PixelMap3d *map;

    public:
        GrowShrink(PixelMap3d *map)
        {
            this->map = map;
            this->name = "Grow shrink";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(params->getVelocity(5000, 500));
            lfo.setDutyCycle(params->getAmount(0.3, 1));
            float size = params->getSize(0.1, 0.4);
            float offset = params->getOffset(0, 1);

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                float lfoSize = lfo.getValue(offset * (map->z(index) + 1) / 2) * size;

                float distance = abs(map->y(index));
                if (distance > lfoSize)
                    continue;

                float distanceAsRatio = 1 - distance / lfoSize;

                pixels[index] = params->getGradient(distanceAsRatio * 255) * distanceAsRatio * transition.getValue();
            }
        }
    };


    class SinChase2Pattern : public Pattern<RGBA>
    {

        Transition transition;
        LFO<Glow> lfo;

    public:
        SinChase2Pattern()
        {
            this->name = "Sin chase 2";
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
                pixels[zigzagIndices[i]] = params->getSecondaryColour() * lfo.getValue(phase) * transition.getValue();
            }
        }
    };

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
        int beatDivs[5] = {16, 8, 4, 2, 2};

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
                        pixels[bar * segmentSize + i] += params->getGradient(255 * fadeValue) * fadeValue * transition.getValue();
                    }
                }
            }
        }
    };

}