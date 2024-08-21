#pragma once
#include "mappingHelpers.hpp"
#include "core/generation/patterns/pattern.hpp"
#include "generation/patterns/helpers/fade.h"
#include "generation/patterns/helpers/interval.h"
#include "generation/patterns/helpers/timeline.h"
#include "generation/pixelMap.hpp"
#include <math.h>
#include <vector>


namespace Mask
{

    class SinChaseMaskPattern : public Pattern<RGBA>
    {

        Transition transition;
        LFO<Glow> lfo;
        uint16_t *remap = nullptr;

    public:
        SinChaseMaskPattern(uint16_t * remap=nullptr)
        {
            this->name = "Sin chase Mask";
            this->remap = remap;
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
                int index=i;
                if (remap) index = remap[index];
                pixels[index] = RGBA(0, 0, 0, 255) * (1. - lfo.getValue(phase)) * transition.getValue();
            }
        }
    };

    class GlowPulseMaskPattern : public Pattern<RGBA>
    {
        Permute perm;
        LFO<Glow> lfo = LFO<Glow>(10000);
        Transition transition;

    public:
        GlowPulseMaskPattern()
        {
            this->name = "Glow pulse mask";
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
                pixels[perm.at[index]] = RGBA(0, 0, 0, 255) * (1. - lfo.getValue(float(index) / width)) * transition.getValue(index, width);
            }
        }
    };

class XY : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap3d *map;
        LFO<Sin> lfoX = LFO<Sin>(2000);
        LFO<Sin> lfoZ = LFO<Sin>(2000);

    public:
        XY(PixelMap3d *map)
        {
            this->map = map;
            this->name="XY";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;
 
            auto col = RGBA(0,0,0,255);
            float size = params->getSize(0.01,0.03);
            lfoX.setPeriod(params->getVelocity(4*20000,2000));
            lfoZ.setPeriod(params->getVelocity(4*14000,1400));

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                float x_norm = (map->x(index)+1)/2;
                float z_norm = (map->z(index)+1)/2;

                float dim = 1.-std::max(
                    softEdge(abs(x_norm - lfoX.getValue()), size),
                    softEdge(abs(z_norm - lfoZ.getValue()*1.1), size)
                );

                pixels[index] = col * dim;
            }
        }
    };

    class HorizontalSaw : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<SawDown> lfo;
        PixelMap3d::Cylindrical *map;

    public:
        HorizontalSaw(PixelMap3d::Cylindrical *map)
        {
            this->map = map;
            this->name = "Horizontal saw";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(params->getVelocity(5000,500));
            lfo.setDutyCycle(params->getSize(0.06,1));

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                RGBA color = RGBA(0,0,0,255); 
                float lfoArg = fromTop(map->z(index));
                pixels[index] = color * (1.-lfo.getValue(lfoArg)) * transition.getValue();
            }
        }
    };

    class RadialSaw : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<SawDown> lfo;
        PixelMap3d::Cylindrical *map;

    public:
        RadialSaw(PixelMap3d::Cylindrical *map)
        {
            this->map = map;
            this->name = "Radial saw";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(params->getVelocity(5000,500));
            lfo.setDutyCycle(params->getSize(0.06,1));

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                RGBA color = RGBA(0,0,0,255);
                float lfoArg = around(map->th(index));
                pixels[index] = color * (1.-lfo.getValue(lfoArg)) * transition.getValue();
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
                    RGBA color = RGBA(0,0,0,255);
                    pixels[i] += color * (1.-fadePosition) * transition.getValue();
                }
            }
        }
    };
};