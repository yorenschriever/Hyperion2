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
    const float l = 60 * 0.005;
    const float r1 = 0.35;
    const float r2 = r1 + 0.5 * l * sqrt(3);
    const float r3 = r1 + l;
    const float r4 = r3 + 0.5 * l * sqrt(3);

    class GlitchPattern : public Pattern<RGBA>
    {
        Permute perm = Permute(0);
        int segmentSize;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        GlitchPattern(int segmentSize=60)
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
            float lfoWidth = segmentSize * factor;

            for (int segment = 0; segment < numSegments; segment++)
            {
                int randomSegment = perm.at[segment];

                for (int j = 0; j < segmentSize; j++)
                {
                    float lfoVal = lfo.getValue(float(j) / lfoWidth + float(randomSegment) / numSegments + float(segment));
                    RGBA col = params->gradient->get(lfoVal * 255) * lfoVal * transition.getValue();
                    pixels[segment * segmentSize + j] = col;
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
                pixels[perm.at[index]] = params->getSecondaryColour() * transition.getValue();
        }
    };

    class HaloOrSwirl : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<Sin> lfoX = LFO<Sin>(2000);
        LFO<Sin> lfoY = LFO<Sin>(2000);
        int modulus;

    public:
        HaloOrSwirl(int modulus=2)
        {
            this->modulus = modulus;
            this->name = modulus==2 ? "Halo": "Swirl";
        }

        void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            for (int index = 0; index < width; index++)
            {
                if ((index / 60) % 3 != modulus)
                    continue;

                pixels[index] = params->getPrimaryColour() * transition.getValue();
            }
        }
    };

    class Skirt : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap::Polar *map;

    public:
        Skirt(PixelMap::Polar *map)
        {
            this->map = map;
            this->name = "Skirt";
        }

        void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                float alpha = 0;
                float r = map->r(index);
                if (r < r2)
                    alpha = Utils::rescale_c(r, 0, 1, r1, r2);
                if (r > r3)
                    alpha = Utils::rescale_c(r, 0, 1, r3, r4);

                pixels[index] = params->getPrimaryColour() * alpha * transition.getValue();
            }
        }
    };

    class XY : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap *map;
        LFO<Sin> lfoX = LFO<Sin>(2000);
        LFO<Sin> lfoY = LFO<Sin>(2000);

    public:
        XY(PixelMap *map)
        {
            this->map = map;
            this->name = "XY";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            auto col = params->getSecondaryColour() * transition.getValue();
            float size = 0.06; // params->getSize(0.01,0.03);
            lfoX.setPeriod(10000 /*params->getVelocity(4*20000,2000)*/);
            lfoY.setPeriod(13000 /*params->getVelocity(4*14000,1400)*/);

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                float x_norm = (map->x(index) + 1) / 2;
                float y_norm = (map->y(index) + 1) / 2;

                float dim = std::max(
                    softEdge(abs(x_norm - lfoX.getValue()), size),
                    softEdge(abs(y_norm - lfoY.getValue()), size));

                pixels[index] = col * dim;
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
                pixels[index] = color * lfo.getValue(-2 * around(map->th(index))) * Utils::rescale(map->r(index), 0.5, 1, 0.4, 1) * transition.getValue();
                // pixels[index] = color * lfo.getValue(fromTop(map->z(index))) * transition.getValue();
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
        int pos = 0;

    public:
        GrowingCirclesPattern(PixelMap *map)
        {
            for (int i = 0; i < 6; i++)
            {
                float xc = cos(float(i) / 6 * 2 * M_PI);
                float yc = sin(float(i) / 6 * 2 * M_PI);
                radii[i].reserve(map->size());
                std::transform(map->begin(), map->end(), std::back_inserter(radii[i]), [xc, yc](PixelPosition pos) -> float
                               { return sqrt(pow(pos.x - xc, 2) + pow(pos.y - yc, 2)); });
            }
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
            for (int i = 0; i < 6; i++)
                fade[i].duration = params->getSize(400, 50) * velocity / 1000;

            for (int i = 0; i < width; i++)
            {
                pixels[i] = RGBA(); // params->getSecondaryColour();
                for (int column = 0; column < 6; column++)
                {
                    float fadePosition = fade[column].getValue(radii[column][i] * velocity);
                    // RGBA color = params->getPrimaryColour();
                    RGBA color = params->gradient->get(255 - 255. * fadePosition);
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
        GlowPulsePattern()
        {
            this->name = "Glow pulse";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            // float density = 481./width;
            int density2 = width / 481;
            if (width == 96)
                density2 = 10;
            lfo.setPeriod(params->getVelocity(10000, 500));
            lfo.setDutyCycle(0.1);
            perm.setSize(width);

            for (int index = 0; index < width; index++)
            {
                pixels[perm.at[index]] = RGBA(); // params->getPrimaryColour();
                if (index % density2 != 0)
                    continue;
                pixels[perm.at[index]] += params->getSecondaryColour() * lfo.getValue(float(index) / width) * transition.getValue(index, width);
            }
        }
    };

    class SpiralPattern : public Pattern<RGBA>
    {
        Transition transition = Transition();
        PixelMap::Polar *map;
        LFO<SinFast> lfo;

    public:
        SpiralPattern(PixelMap::Polar *map)
        {
            this->map = map;
            this->name = "Spiral";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(/*params->getVelocity(5000,300)*/ 1000);
            int amount = params->getAmount(1, 3);
            float curliness = 1; // params->getVariant(0.5,3);
            float size = /*params->getSize(0.01,0.1)*/ 0.1 * amount + curliness / 24;

            for (int i = 0; i < width; i++)
            {
                float spiral = amount * (around(map->th(i)) + map->r(i) * curliness);
                // while (spiral < 0) spiral += 1;
                // while (spiral > 1) spiral -= 1;
                float pos = abs(spiral - lfo.getPhase());

                // for some reason pos can become very large. so large that  subtracting 1
                // fall withing the rounding accuracy, and the loop below becomes an infinite loop.
                // pos is never expected to be bigger than only a few units, so if we some something else,
                // i just ignore this frame.
                if (pos > 10)
                    return;
                while (pos > 1)
                {
                    pos -= 1;
                }
                float fadePosition = softEdge(pos, size, 0.06);
                RGBA color = params->gradient->get(255 - 255 * map->r(i));

                pixels[i] = color * fadePosition * transition.getValue();
            }
        }
    };

    class GrowingCirclesPattern2 : public Pattern<RGBA>
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
        Timeline timeline = Timeline(200);
        int pos = 0;

    public:
        GrowingCirclesPattern2(PixelMap *map)
        {
            // Log::info("","Constructor GrowingCirclesPattern2");
            for (int i = 0; i < 6; i++)
            {
                float xc = 0.5 * cos(float(i) / 6 * 2 * M_PI);
                float yc = 0.5 * sin(float(i) / 6 * 2 * M_PI);
                // Log::info("","Reserve %d, freeheap = %d, map size = %d, float size = %d",i,Utils::get_free_heap(), map->size(), sizeof(float));
                // radii[i].reserve(map.size()*sizeof(float));
                radii[i].reserve(map->size());
                // Log::info("","fill %d, freeheap = %d",i,Utils::get_free_heap());
                std::transform(map->begin(), map->end(), std::back_inserter(radii[i]), [xc, yc](PixelPosition pos) -> float
                               { return sqrt(pow(pos.x - xc, 2) + pow(pos.y - yc, 2)); });
                // Log::info("","end %d, freeheap = %d",i,Utils::get_free_heap());
            }
            // this->perm = Permute(map.size());
            this->name = "Growing circles";
            Log::info("", "End Constructor GrowingCirclesPattern2");
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            timeline.FrameStart();
            if (timeline.Happened(0))
            {
                pos = (pos + 1) % 6;
                fade[pos].reset();
            }

            float velocity = params->getVelocity(1000, 100);
            for (int i = 0; i < 6; i++)
                fade[i].duration = params->getSize(700, 50) * velocity / 1000;

            for (int i = 0; i < width; i++)
            {
                pixels[i] = RGBA();
                for (int column = 0; column < 6; column++)
                {
                    float fadePosition = fade[column].getValue(radii[column][i] * velocity);
                    RGBA color = params->gradient->get(255 - 255. * radii[column][i]); //  params->getPrimaryColour();
                    pixels[i] += color * fadePosition * transition.getValue();
                }
            }
        }
    };

    class ChevronsPattern : public Pattern<RGBA>
    {
        PixelMap *map;
        LFO<SawDown> lfo;
        LFO<Square> lfoColour;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        FadeDown fade1 = FadeDown(1400);

    public:
        ChevronsPattern(PixelMap *map)
        {
            this->map = map;
            this->lfo = LFO<SawDown>(1000);
            this->lfoColour = LFO<Square>(1000);
            this->name = "Chevrons";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            float amount = params->getAmount(0.25, 4);
            lfo.setPeriod(params->getVelocity(2000, 500));
            lfoColour.setPeriod(params->getOffset(1000, 500));

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                // float dir = (map->z(index) > 0 || map->y(index) > 0.44) ? 1:-1;
                float phase = (0.5 * abs(map->x(index)) + map->y(index)) * amount;
                auto col = lfoColour.getValue(phase) ? params->getSecondaryColour() : params->getPrimaryColour();
                pixels[index] += col * lfo.getValue(phase) * transition.getValue();
            }
        }
    };

    class ChevronsConePattern : public Pattern<RGBA>
    {
        PixelMap::Polar *map;
        LFO<SawDown> lfo;
        LFO<Square> lfoColour;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        ChevronsConePattern(PixelMap::Polar *map)
        {
            this->map = map;
            this->name = "Chevrons cone";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            float amount = params->getAmount(0.25, 4);
            lfo.setPeriod(params->getVelocity(2000, 500));
            lfoColour.setPeriod(params->getOffset(1000, 500));

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                float conePos = 0.5 + (map->r(index)) / 2;

                float phase = conePos * amount;
                auto col = (lfoColour.getValue(phase) > 0) ? params->getSecondaryColour() : params->getPrimaryColour();
                pixels[index] += col * lfo.getValue(phase) * transition.getValue();
            }
        }
    };

    class RadialGlitterFadePattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap::Polar *map;
        FadeDown fade1 = FadeDown(200);
        FadeDown fade2 = FadeDown(200);
        BeatWatcher watcher = BeatWatcher();
        Permute perm1;
        Permute perm2;
        int fadeNr = 0;

    public:
        RadialGlitterFadePattern(PixelMap::Polar *map)
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

            fade1.duration = params->getIntensity(500, 100);
            fade2.duration = params->getIntensity(500, 100);

            // timeline.FrameStart();
            // if (timeline.Happened(0))
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
            // float trail = params->getIntensity(0,1) * density;
            // float trail = params->getIntensity(0, 200);
            float fromCenter = params->getVariant() < 0.5;

            if (perm1.at == 0 || perm2.at == 0)
                return;

            for (int i = 0; i < map->size(); i++)
            {
                // fade.duration = perm.at[i] * trail; // + 100;
                // fade.duration = (perm.at[i] * trail) < map.size() / 20 ? ; // + 100;

                // fade.duration = 100;
                // if (perm.at[i] < density * map.size()/ 10)
                //     fade.duration *= 4;

                float density = 481. / width;
                fade1.duration = 100; // trail + perm.at[i] / (density * map.size()/ 10);
                fade2.duration = 100;
                if (perm1.at[i] < density * map->size() / 10)
                    fade1.duration *= perm1.at[i] * 4 / (density * map->size() / 10);
                if (perm2.at[i] < density * map->size() / 10)
                    fade2.duration *= perm2.at[i] * 4 / (density * map->size() / 10);

                float conePos = fromCenter ?
                                           // vanaf midden
                                    (0.5 + (map->r(i)) / 2)
                                           :
                                           // vanaf de knik beide kanten op
                                    (1 - (map->r(i)) / 2);

                float fadePosition1 = fade1.getValue(conePos * velocity);
                RGBA color1 = params->gradient->get(fadePosition1 * 255);
                pixels[i] = color1 * fadePosition1 * (1.5 - map->r(i)) * transition.getValue();

                float fadePosition2 = fade2.getValue(conePos * velocity);
                RGBA color2 = params->gradient->get(fadePosition2 * 255);
                pixels[i] += color2 * fadePosition2 * (1.5 - map->r(i)) * transition.getValue();
            }
        }
    };

    class AngularFadePattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap::Polar *map;
        static const int numFades = 4;
        int fadeNr = 0;
        FadeDown fade[numFades] = {
            FadeDown(200),
            FadeDown(200),
            FadeDown(200),
            FadeDown(200)};
        BeatWatcher watcher = BeatWatcher();

    public:
        AngularFadePattern(PixelMap::Polar *map)
        {
            this->map = map;
            this->name = "Angular fade";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            int beatDiv = params->getVariant(0, 6);
            if (beatDiv > 3)
                beatDiv = 3;
            int divs[] = {8, 4, 2, 1};

            if (watcher.Triggered() && Tempo::GetBeatNumber() % divs[beatDiv] == 0)
            {
                fadeNr = (fadeNr + 1) % numFades;
                fade[fadeNr].reset();
                // perm.permute();
            }

            // float density = 481./width;
            float velocity = params->getVelocity(1000, 30);
            float trail = params->getSize(40, 200);

            for (int f = 0; f < numFades; f++)
            {
                fade[f].duration = trail;
            }

            for (int i = 0; i < map->size(); i++)
            {
                // fade.duration = 80; // trail + perm.at[i] / (density * map.size()/ 10);
                //  if (perm.at[i] < density * map.size()/ 10)
                //      fade.duration *= perm.at[i] * 4 / (density * map.size()/ 10);
                // pixels[i] = params->getSecondaryColour() * 0.25;
                for (int f = 0; f < numFades; f++)
                {
                    float fadePosition = fade[f].getValue(abs(map->th(i)) * velocity);
                    RGBA color = params->gradient->get(255 - abs(map->th(i)) / M_PI * 255);
                    pixels[i] += color * fadePosition * (map->th(i) * 1.5) * transition.getValue();
                }
            }
        }
    };

    class RadialFadePattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap::Polar *map;
        FadeDown fade = FadeDown(200);
        BeatWatcher watcher = BeatWatcher();

    public:
        RadialFadePattern(PixelMap::Polar *map)
        {
            this->map = map;
            this->name = "Radial fade";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            fade.duration = params->getSize(500, 120);
            int velocity = params->getVelocity(500, 50);

            if (!transition.Calculate(active))
                return;

            if (watcher.Triggered())
            {
                fade.reset();
            }

            for (int i = 0; i < map->size(); i++)
            {
                // pixels[i] = params->getSecondaryColour();
                float conePos = 0.20 + (map->r(i)) / 2;
                pixels[i] += params->getPrimaryColour() * fade.getValue(conePos * velocity) * transition.getValue();
            }
        }
    };

    class Quadrants : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap *map;

    public:
        Quadrants(PixelMap *map)
        {
            this->map = map;
            this->name = "Quadrant";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                RGBA col;
                if (map->x(index) < 0)
                    col = map->y(index)< 0? RGB(255,0,0) : RGB(0,255,0);
                else
                    col = map->y(index)< 0? RGB(0,0,255) : RGB(255,255,0);

                pixels[index] = col * transition.getValue();
            }
        }
    };

    class Quadrants3d : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap3d *map;

    public:
        Quadrants3d(PixelMap3d *map)
        {
            this->map = map;
            this->name = "Quadrant";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                RGBA col = RGB(
                    map->x(index) < 0 ? 255:0,
                    map->y(index) < 0 ? 255:0,
                    map->z(index) < 0 ? 255:0
                );

                pixels[index] = col * transition.getValue();
            }
        }
    };

}