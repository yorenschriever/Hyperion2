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

namespace Mid
{

    class Lighthouse : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<LFOPause<NegativeCosFast>> lfo;
        PixelMap3d::Cylindrical map;

    public:
        Lighthouse(PixelMap3d::Cylindrical map)
        {
            this->map = map;
            this->name = "Lighthouse";
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
                pixels[index] = color * lfo.getValue(-2 * around(map[index].th)) * fromMid(map[index]) * transition.getValue();
                // pixels[index] = color * lfo.getValue(fromTop(map[index].z)) * transition.getValue();
            }
        }
    };

    class Halo : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap3d::Cylindrical map;

    public:
        Halo(PixelMap3d::Cylindrical map)
        {
            this->map = map;
            this->name = "Halo";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            auto col = params->getSecondaryColour() * transition.getValue();

            for (int index = 0; index < std::min(width, (int)map.size()); index++)
            {
                if (map[index].z < 0.44)
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
        PixelMap3d::Cylindrical map;

    public:
        Halo2(PixelMap3d::Cylindrical map)
        {
            this->map = map;
            this->name = "Halo 2";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            auto col = params->getSecondaryColour() * transition.getValue();

            for (int index = 0; index < std::min(width, (int)map.size()); index++)
            {
                if (map[index].r < 0.6)
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
            FadeDown(2000, WaitAtEnd),
            FadeDown(2000, WaitAtEnd),
            FadeDown(2000, WaitAtEnd),
            FadeDown(2000, WaitAtEnd)};
        PixelMap3d::Cylindrical map;
        BeatWatcher watcher = BeatWatcher();
        int pos = 0;

    public:
        HaloOnBeat(PixelMap3d::Cylindrical map)
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

            if (width != 481)
            {
                // columns
                for (int index = 0; index < std::min(width, (int)map.size()); index++)
                {
                    if (map[index].z < 0.44)
                        continue;
                    pixels[index] = col * fade[0].getValue();
                }
                return;
            }

            // ledster
            for (auto petal : LedsterShapes::petals)
            {
                for (int j = 18; j < 37; j++)
                {
                    // float dist = 1 - abs((float)j - 9 + 5 - 45. / 2) / 10;
                    pixels[petal[j]] = col * fade[1].getValue();
                }
            }

            auto hex = LedsterShapes::hexagons[9];
            for (int j = 0; j < hex.size(); j++)
            {
                // float dist = abs(float(j % (hex.size() / 6)) - (hex.size() / 12) - 0.5) / 4;
                pixels[hex[j]] = col * fade[2].getValue();
            }

            hex = LedsterShapes::hexagons[3];
            for (int j = 0; j < hex.size(); j++)
            {
                // float dist = abs(float(j % (hex.size() / 6)) - (hex.size() / 12) - 0.5) / 2;
                pixels[hex[j]] = col * fade[3].getValue();
                ;
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

    class SnowflakePatternColumn : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap3d::Cylindrical map;

    public:
        SnowflakePatternColumn(PixelMap3d::Cylindrical map)
        {
            this->map = map;
            this->name = "Snowflake";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            for (int index = 0; index < std::min(width, (int)map.size()); index++)
            {
                if (map[index].z < 0.44)
                    continue;

                pixels[index] = params->getPrimaryColour() * transition.getValue() * Utils::rescale(map[index].r, 0, 1, 1.02, 1.15);
            }
        }
    };

    class TakkenChase : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<LFOPause<SawDown>> lfo;
        PixelMap3d::Cylindrical map;

    public:
        TakkenChase(PixelMap3d::Cylindrical map)
        {
            this->map = map;
            this->name = "Takken chase";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            int amount = params->getAmount(1, 12);
            lfo.setPeriod(params->getVelocity(5000, 500));
            lfo.setPulseWidth(params->getSize(0.06, 0.5));

            for (int index = 0; index < std::min(width, (int)map.size()); index++)
            {
                // skip the vertical beams. this pattern uses only the horizontal and angled ones
                int angle = around(map[index].th) * 3600;
                if ((angle + 300) % 600 == 0)
                    continue;

                float lfoVal = lfo.getValue(-amount * around(map[index].th));
                RGBA color = params->gradient->get(lfoVal * 255);
                pixels[index] = color * lfoVal * fromMid(map[index]) * transition.getValue();
            }
        }
    };

    class PetalChase : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap3d::Cylindrical map;
        LFO<LFOPause<SawDown>> lfo1;

    public:
        PetalChase(PixelMap3d::Cylindrical map)
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
            lfo1.setPulseWidth(params->getSize(0.06, 0.5));

            for (int index = 0; index < std::min(width, (int)map.size()); index++)
            {
                if (map[index].r < 0.35)
                    continue;

                float lfoVal1 = lfo1.getValue(-amount1 * around(map[index].th));
                RGBA color1 = params->gradient->get(lfoVal1 * 255);

                pixels[index] = color1 * lfoVal1 * transition.getValue();
            }
        }
    };

    class DoubleFlash : public Pattern<RGBA>
    {
        // Transition transition = Transition(
        //     200, Transition::none, 0,
        //     1000, Transition::none, 0);
        PixelMap3d::Cylindrical map;
        // LFO<LFOPause<SawDown>> lfo1;
        Timeline timeline = Timeline();
        BeatWatcher watcher;

    public:
        DoubleFlash(PixelMap3d::Cylindrical map)
        {
            this->map = map;
            this->name = "Double flash 2/4";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            timeline.FrameStart();

            if (watcher.Triggered() && Tempo::GetBeatNumber() % 4 == 2)
                timeline.reset();

            if (!timeline.Happened(0) && !timeline.Happened(100))
                return;

            for (int i = 0; i < width; i++)
            {
                if (map[i].z < 0.44)
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
        FireworksPattern(PixelMap3d map)
        {
            for (int i = 0; i < 6; i++)
            {
                float xc = cos(float(i) / 6 * 2 * M_PI);
                float yc = 0.30;
                float zc = sin(float(i) / 6 * 2 * M_PI);
                std::transform(map.begin(), map.end(), std::back_inserter(radii[i]), [xc, yc, zc](PixelPosition3d pos) -> float
                               { return sqrt(pow(pos.x - xc, 2) + pow(pos.y - yc, 2) + pow(pos.z - zc, 2)); });
            }
            this->perm = Permute(map.size());
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

            float size = params->getSize(0.1,0.5);

            for (int i = 0; i < width; i++)
            {
                for (int column = 0; column < 6; column++)
                {
                    if (radii[column][i] > size)
                        continue;
                    float fadePosition = fade[column].getValue(radii[column][i] * velocity);
                    float fadeRatio = 1. - (radii[column][i] / size);
                    RGBA color = params->gradient->get(255 * fadeRatio) * fadeRatio;
                    pixels[i] += color * fadePosition * transition.getValue();
                }
            }
        }
    };
}