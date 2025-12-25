#pragma once
#include "pattern.hpp"
#include "utils.hpp"
#include "core/generation/pixelMap/pixelMap.hpp"
#include "helpers/fade.h"
#include <vector>
#include <math.h>

namespace Mapped
{

    template <class T>
    class ConcentricWavePattern : public Pattern<RGBA>
    {
        PixelMap *map;
        int numWaves;
        LFO<T> lfo;
        LFO<Square> lfoColour;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        ConcentricWavePattern(PixelMap *map, int numWaves = 1, int numColourWaves=2, int period = 5000)
        {
            this->map = map;
            this->numWaves = numWaves;
            this->lfo = LFO<T>(period);
            this->lfoColour = LFO<Square>(period / numColourWaves);
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return;

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                float distance = std::abs(map->x(index)) + std::abs(map->y(index));
                float phase = (float)numWaves * distance;
                RGBA colour = lfoColour.getValue(phase) ? params->getSecondaryColour() : params->getPrimaryColour();
                RGBA dimmedColour = colour * transition.getValue() * lfo.getValue(phase);
                pixels[index] += dimmedColour;
            }
        }
    };

    template <class T>
    class HorizontalWavePattern : public Pattern<RGBA>
    {
        PixelMap *map;
        LFO<T> lfo;
        // LFO<Square> lfoColour;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        HorizontalWavePattern(PixelMap *map, int period = 5000)
        {
            this->map = map;
            this->lfo = LFO<T>(period);
            // this->lfoColour = LFO<Square>(period / 2);
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return;

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                // float distance = std::abs(map->x(index)) + std::abs(map->y(index));
                float phase = (map->x(index) + 1) / 2.;
                RGBA colour = params->getSecondaryColour(); // lfoColour.getValue(phase) ? params->getSecondaryColour() : params->getPrimaryColour();
                RGBA dimmedColour = colour * transition.getValue() * lfo.getValue(phase);
                pixels[index] += dimmedColour;
            }
        }
    };

    //template <class T>
    class ConcentricPulsePattern : public Pattern<RGBA>
    {
        PixelMap *map;
        Fade<Down, Linear> fades[5] = {
            Fade<Down, Linear>(200),
            Fade<Down, Linear>(200),
            Fade<Down, Linear>(200),
            Fade<Down, Linear>(200),
            Fade<Down, Linear>(200)};
        int triggerNext = 0;
        bool lastActive = false;

    public:
        ConcentricPulsePattern(PixelMap *map)
        {
            this->map = map;
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (active && !lastActive)
            {
                fades[triggerNext].reset();
                triggerNext = (triggerNext + 1) % 5;
            }
            lastActive = active;

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                float distance = std::abs(map->x(index)) + std::abs(map->y(index));
                int delay = distance * 200;
                float l = fades[0].getValue(delay) +
                          fades[1].getValue(delay) +
                          fades[2].getValue(delay) +
                          fades[3].getValue(delay) +
                          fades[4].getValue(delay);
                if (l > 1)
                    l = 1;

                pixels[index] += params->getHighlightColour() * l;
            }
        }
    };

    class RadarPattern : public Pattern<RGBA>
    {
        PixelMap *map;
        LFO<SawDown> lfo;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        std::vector<float> scaledAngles;

    public:
        RadarPattern(PixelMap *map, int period = 5000)
        {
            this->map = map;
            this->lfo = LFO<SawDown>(period);
            //this->lfo.setSkew(0.25);
            std::transform(map->begin(), map->end(), std::back_inserter(scaledAngles), [](PixelPosition pos) -> float
                           { return (atan2(pos.y, pos.x) + M_PI) / (2 * M_PI); });
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return;

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                RGBA colour = params->getSecondaryColour();
                float lfoVal = lfo.getValue(scaledAngles[index]);
                RGBA dimmedColour = colour * transition.getValue() * lfoVal;
                pixels[index] += dimmedColour;
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
            this->map = map;
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return;

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                float phase = (map->x(index) + 1) / 2.;
                RGBA colour = params->getSecondaryColour() + params->getPrimaryColour() * phase;
                RGBA dimmedColour = colour * transition.getValue();
                pixels[index] += dimmedColour;
            }
        }
    };

    class HorizontalDitheredGradientPattern : public Pattern<RGBA>
    {
        PixelMap *map;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        std::vector<float> noise;
        const size_t maxNoiseMapSize = 100;

    public:
        HorizontalDitheredGradientPattern(PixelMap *map)
        {
            this->map = map;
            for (int i = 0; i < std::min(map->size(), maxNoiseMapSize); i++)
            {
                noise.push_back((Utils::random_f() - 0.5) * 1.3);
            }
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return;

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                RGBA colour = (map->x(index) + noise[index % maxNoiseMapSize] < 0) ? params->getSecondaryColour() : params->getPrimaryColour();
                RGBA dimmedColour = colour * transition.getValue();
                pixels[index] += dimmedColour;
            }
        }
    };

    template <class T>
    class DiagonalWavePattern : public Pattern<RGBA>
    {
        PixelMap *map;
        LFO<T> lfo;
        float numWaves;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        DiagonalWavePattern(PixelMap *map, int period = 5000, float numWaves = 1, float pulseWidth = 0.5, float skew = 1)
        {
            this->map = map;
            this->lfo = LFO<T>(period);
            this->numWaves = numWaves;
            this->lfo.setDutyCycle(pulseWidth);
            //this->lfo.setSkew(skew);
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return;

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                float phase = (map->x(index) + map->y(index) + 2) * numWaves;
                RGBA colour = params->getPrimaryColour() + params->getSecondaryColour() * lfo.getValue(phase); 
                RGBA dimmedColour = colour * transition.getValue();
                pixels[index] += dimmedColour;
            }
        }
    };

} // namespace Mapped