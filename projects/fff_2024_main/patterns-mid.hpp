#pragma once
#include "mappingHelpers.hpp"
#include <math.h>
#include <vector>

namespace Mid
{

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
                pixels[index] = color * lfo.getValue(-2 * around(map->th(index))) * fromMid(map->operator[](index)) * transition.getValue();
                // pixels[index] = color * lfo.getValue(fromTop(map->z(index))) * transition.getValue();
            }
        }
    };

    class Halo : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap3d::Cylindrical *map;

    public:
        Halo(PixelMap3d::Cylindrical *map)
        {
            this->map = map;
            this->name = "Halo";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            auto col = params->getSecondaryColour() * transition.getValue();

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                if (map->z(index) < 0.44)
                    continue;

                pixels[index] = col;
            }
        }
    };

    class Halo2 : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap3d::Cylindrical *map;

    public:
        Halo2(PixelMap3d::Cylindrical *map)
        {
            this->map = map;
            this->name = "Halo 2";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            auto col = params->getSecondaryColour() * transition.getValue();

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                if (map->r(index) < 0.6)
                    continue;

                pixels[index] = col;
            }
        }
    };

    class HaloOnBeat : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        FadeDown fade[4] = {
            FadeDown(2000),
            FadeDown(2000),
            FadeDown(2000),
            FadeDown(2000)};
        PixelMap3d::Cylindrical *map;
        BeatWatcher watcher = BeatWatcher();
        int pos = 0;

    public:
        HaloOnBeat(PixelMap3d::Cylindrical *map)
        {
            this->map = map;
            this->name = "Halo on beat";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            if (watcher.Triggered())
            {
                pos = (pos + 1) % 4;
                fade[pos].reset();
            }

            float duration = params->getVelocity(2000, 100);
            for (int i = 0; i < 4; i++)
                fade[i].duration = duration;

            auto col = params->getPrimaryColour() * transition.getValue();

            // columns
            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                if (map->z(index) < 0.44)
                    continue;
                pixels[index] = col * fade[0].getValue();
            }
        }
    };

    class SnowflakePatternColumn : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap3d::Cylindrical *map;

    public:
        SnowflakePatternColumn(PixelMap3d::Cylindrical *map)
        {
            this->map = map;
            this->name = "Snowflake";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                if (map->z(index) < 0.44)
                    continue;

                pixels[index] = params->getPrimaryColour() * transition.getValue() * Utils::rescale(map->r(index), 0, 1, 1.02, 1.15);
            }
        }
    };

    class TopChase : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<SawDown> lfo;
        PixelMap3d::Cylindrical *map;

    public:
        TopChase(PixelMap3d::Cylindrical *map)
        {
            this->map = map;
            this->name = "Top chase";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            int amount = params->getAmount(1, 12);
            lfo.setPeriod(params->getVelocity(5000, 500));
            lfo.setDutyCycle(params->getSize(0.06, 0.5));

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                float lfoVal = lfo.getValue(-amount * around(map->th(index)));
                RGBA color = params->getGradient(lfoVal * 255);
                pixels[index] = color * lfoVal * fromTop(map->operator[](index).z) * transition.getValue();
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

            int amount1 = params->getAmount(1, 12);
            lfo1.setPeriod(params->getVelocity(5000, 500));
            lfo1.setDutyCycle(params->getSize(0.06, 0.5));
            lfo1.setSoftEdgeWidth(0.1);

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                if (map->r(index) < 0.35)
                    continue;

                float lfoVal1 = lfo1.getValue(-amount1 * around(map->th(index)));
                RGBA color1 = params->getGradient(lfoVal1 * 255);

                pixels[index] = color1 * lfoVal1 * transition.getValue();
            }
        }
    };

    class DoubleFlash : public Pattern<RGBA>
    {
        PixelMap3d::Cylindrical *map;
        Timeline timeline = Timeline();
        BeatWatcher watcher;

    public:
        DoubleFlash(PixelMap3d::Cylindrical *map)
        {
            this->map = map;
            this->name = "Double flash";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            timeline.FrameStart();

            int beatDiv = params->getAmount(0,4);
            if (beatDiv >3) beatDiv =3;
            int divs[] = {8,4,2,1};

            if (watcher.Triggered() && Tempo::GetBeatNumber() % divs[beatDiv] == 0)
                timeline.reset();

            if (!timeline.Happened(0) && !timeline.Happened(100))
                return;

            for (int i = 0; i < width; i++)
            {
                if (map->z(i) < 0.44)
                    continue;
                pixels[i] = params->getHighlightColour();
            }
        }
    };

    class FireworksPattern : public Pattern<RGBA>
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
        FireworksPattern(PixelMap3d *map)
        {
            for (int i = 0; i < 6; i++)
            {
                float xc = cos(float(i) / 6 * 2 * M_PI);
                float yc = 0.30;
                float zc = sin(float(i) / 6 * 2 * M_PI);
                std::transform(map->begin(), map->end(), std::back_inserter(radii[i]), [xc, yc, zc](PixelPosition3d pos) -> float
                               { return sqrt(pow(pos.x - xc, 2) + pow(pos.y - yc, 2) + pow(pos.z - zc, 2)); });
            }
            this->perm = Permute(map->size());
            this->name = "Fireworks";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            int amount = params->getAmount(0,4);

            int beatDivisions[] = {8,4,2,1,1};
            int beatDivision = beatDivisions[(int)params->getIntensity(0,4)];
            if (watcher.Triggered() && Tempo::GetBeatNumber() % beatDivision == 0)
            {
                pos = (pos + 1) % 6;
                if (amount ==3 || amount==4)
                    for (int i = 0; i < 6; i++) 
                        fade[i].reset();
                else if (amount == 2)
                    for (int i = 0; i < 3; i++) 
                        fade[(i * 2 + pos )%6].reset();
                else if (amount == 1)
                    for (int i = 0; i < 2; i++) 
                        fade[(i * 3 + pos )%6].reset();
                else if (amount == 0)
                        fade[pos].reset();
            }

            float velocity = params->getVelocity(4000,750); 
            for (int i = 0; i < 6; i++)
                fade[i].duration = params->getSize(100, 500);

            float size = params->getSize(0.5,1.5);

            for (int i = 0; i < width; i++)
            {
                for (int column = 0; column < 6; column++)
                {
                    if (radii[column][i] > size)
                        continue;
                    float fadePosition = fade[column].getValue(radii[column][i] * velocity);
                    float fadeRatio = 1. - (radii[column][i] / size);
                    RGBA color = params->getGradient(255 * fadeRatio) * fadeRatio;
                    pixels[i] += color * fadePosition * transition.getValue();
                }
            }
        }
    };
}