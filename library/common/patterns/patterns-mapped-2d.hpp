
#pragma once
#include "hyperion.hpp"
// #include "mappingHelpers.hpp"
#include <math.h>
#include <vector>

namespace Mapped2dPatterns
{

    class Lighthouse : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<Glow> lfo;
        PixelMap::Polar *map;

    public:
        Lighthouse(PixelMap::Polar *map)
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
                RGBA color = params->getSecondaryColor();
                pixels[index] = color * lfo.getValue(-2 * around(map->th(index))) * Utils::rescale(map->r(index), 0, 1, 0, .45) * transition.getValue();
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
        Permute perm;
        int pos = 0;

    public:
        GrowingCirclesPattern(PixelMap *map)
        {
            for (int i = 0; i < 6; i++)
            {
                float xc = cos(float(i) / 6 * 2 * M_PI);
                float yc = sin(float(i) / 6 * 2 * M_PI);
                std::transform(map->begin(), map->end(), std::back_inserter(radii[i]), [xc, yc](PixelPosition pos) -> float
                               { return sqrt(pow(pos.x - xc, 2) + pow(pos.y - yc, 2)); });
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
            for (int i = 0; i < 6; i++)
                fade[i].duration = params->getSize(400, 50) * velocity / 1000;

            // float density = 481./width;

            for (int i = 0; i < width; i++)
            {
                for (int column = 0; column < 6; column++)
                {
                    float fadePosition = fade[column].getValue(radii[column][i] * velocity);
                    RGBA color = params->getPrimaryColor();
                    pixels[i] += color * fadePosition * transition.getValue();
                }
            }
        }
    };

    class LineLaunch : public Pattern<RGBA>
    {
        FadeDown fade = FadeDown(50);
        BeatWatcher watcher = BeatWatcher();
        PixelMap *map;

    public:
        LineLaunch(PixelMap *map)
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
                float fadePosition = fade.getValue((1 + map->y(i)) * velocity);
                RGBA color = params->getSecondaryColor();
                pixels[i] += color * fadePosition;
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

            lfo.setPeriod(params->getVelocity(5000, 300));
            int amount = params->getAmount(1, 3);
            float curliness = params->getVariant(0.5, 3);
            float size = params->getSize(0.01, 0.1) * amount + curliness / 24;

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
                RGBA color = params->getGradient(255 - 255 * map->r(i));

                pixels[i] = color * fadePosition * transition.getValue();
            }
        }
    };

    class DotBeatPattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap::Polar *map;
        FadeDown fade = FadeDown(200);
        BeatWatcher watcher = BeatWatcher();

    public:
        DotBeatPattern(PixelMap::Polar *map)
        {
            this->map = map;
            this->name = "Dot beat";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            fade.duration = params->getIntensity(500, 100);

            if (watcher.Triggered())
                fade.reset();

            for (int i = 0; i < map->size(); i++)
            {
                float radius = fade.getValue() * 0.95;
                if (map->r(i) > radius)
                    continue;

                RGBA color = params->getGradient(radius * 255);
                float dim = map->r(i) / radius;
                pixels[i] = color * dim * transition.getValue();
            }
        }
    };

    class HorizontalSin : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<Glow> lfo;
        PixelMap::Polar *map;

    public:
        HorizontalSin(PixelMap::Polar *map)
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
            int amount = params->getAmount(1, 7.99);

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                RGBA color = params->getGradient(fromTop(map->r(index)) * 255);
                pixels[index] = color * lfo.getValue(amount * around(map->th(index))) * transition.getValue();
            }
        }
    };

    class HorizontalSaw : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<SawDown> lfo;
        PixelMap::Polar *map;

    public:
        HorizontalSaw(PixelMap::Polar *map)
        {
            this->map = map;
            this->name = "Horzontal saw";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(params->getVelocity(5000, 500));
            lfo.setDutyCycle(params->getSize(0.06, 1));

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                RGBA color = params->getPrimaryColor();
                float lfoArg = fromTop(map->r(index));
                pixels[index] = color * lfo.getValue(lfoArg) * transition.getValue();
            }
        }
    };

    class RadialSaw : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<SawDown> lfo;
        PixelMap::Polar *map;

    public:
        RadialSaw(PixelMap::Polar *map)
        {
            this->map = map;
            this->name = "Radial saw";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(params->getVelocity(5000, 500));
            lfo.setDutyCycle(params->getSize(0.06, 1));

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                RGBA color = params->getPrimaryColor();
                float lfoArg = around(map->th(index));
                pixels[index] = color * lfo.getValue(lfoArg) * transition.getValue();
            }
        }
    };

    class GrowShrink : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<SinFast> lfo;
        PixelMap::Polar *map;

    public:
        GrowShrink(PixelMap::Polar *map)
        {
            this->map = map;
            this->name = "Grow shrink";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(params->getVelocity(5000, 500));
            float size = params->getSize(0.1, 1.5);
            int offset = params->getOffset(0, 3.99);

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                float lfoSize = lfo.getValue(Utils::modulus_f(offset * around(map->th(index)))) * size;
                float distance = abs(map->r(index) + 0.07);
                if (distance > lfoSize)
                    continue;

                float distanceAsRatio = 1 - distance / lfoSize;

                pixels[index] = params->getGradient(distanceAsRatio * 255) * distanceAsRatio * transition.getValue();
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
        // std::vector<float> normalizedRadii;
        BeatWatcher watcher = BeatWatcher();

    public:
        RadialFadePattern(PixelMap::Polar *map)
        {
            this->name = "Radial fade";
            this->map = map;

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

            for (int i = 0; i < map->size(); i++)
            {
                pixels[i] += params->getPrimaryColor() * fade.getValue(map->r(i) * velocity) * transition.getValue();
            }
        }
    };



    class RadialGlitterFadePattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap::Polar *map;
        FadeDown fade = FadeDown(200);
        BeatWatcher watcher = BeatWatcher();
        Permute perm;

    public:
        RadialGlitterFadePattern(PixelMap::Polar *map)
        {
            this->name = "Radial glitter fade";
            this->map = map;
            this->perm = Permute(map->size());
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

            for (int i = 0; i < map->size(); i++)
            {

                float density = 481. / width;
                fade.duration = 100; // trail + perm.at[i] / (density * map->size()/ 10);
                if (perm.at[i] < density * map->size() / 10)
                    fade.duration *= perm.at[i] * 4 / (density * map->size() / 10);

                float conePos = 0.5 + (map->r(i)) / 2;

                float fadePosition = fade.getValue(conePos * velocity);
                RGBA color = params->getGradient(fadePosition * 255);
                pixels[i] = color * fadePosition * (1.5 - map->r(i)) * transition.getValue();
            }
        }
    };

    class HorizontalGradientPattern : public Pattern<RGBA>
    {
        PixelMap *map;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        HorizontalGradientPattern(PixelMap *map)
        {
            this->name = "Horizontal gradient";
            this->map = map;
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                RGBA color = params->getGradient(abs(map->x(index)) * 255);
                RGBA dimmedColor = color * transition.getValue();
                pixels[index] += dimmedColor;
            }
        }
    };

    

    class AngularFadePattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap::Polar *map;
        FadeDown fade = FadeDown(200);
        BeatWatcher watcher = BeatWatcher();

    public:
        AngularFadePattern(PixelMap::Polar *map)
        {
            this->name = "Angular fade";
            this->map = map;
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            if (watcher.Triggered())
            {
                fade.reset();
            }

            float velocity = params->getVelocity(500, 100);
            
            for (int i = 0; i < map->size(); i++)
            {
                fade.duration = 80; 
                float th = M_PI - abs(map->th(i));

                float fadePosition = fade.getValue(th * velocity);
                RGBA color = params->getGradient(255 - th / M_PI * 255);
                pixels[i] = color * fadePosition * (map->r(i) * 1.5) * transition.getValue();

            }
        }
    };

    

}
