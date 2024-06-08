#pragma once
#include "core/generation/patterns/pattern.hpp"
#include "generation/patterns/helpers/fade.h"
#include "generation/patterns/helpers/interval.h"
#include "generation/patterns/helpers/timeline.h"
#include "generation/pixelMap.hpp"
#include <math.h>
#include <vector>


namespace Masks
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

    // class SideWaveMask : public Pattern<RGBA>
    // {
    //     Transition transition = Transition(
    //         200, Transition::none, 0,
    //         1000, Transition::none, 0);
    //     LFO<Glow> lfo;
    //     static const int segmentSize = 50;
    //     uint16_t *remap = nullptr;

    // public:
    //     SideWaveMask(uint16_t *remap)
    //     {
    //         this->name = "Side wave mask";
    //         this->remap = remap;
    //     }

    //     inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    //     {
    //         if (!transition.Calculate(active))
    //             return;

    //         float offset = params->getOffset(0, 3);
    //         float duty = params->getAmount(0.1, 1);
    //         lfo.setDutyCycle(duty);
    //         lfo.setPeriod(params->getVelocity(3000, 500) / duty);
    //         float size = params->getSize();
    //         float baseSize = params->getIntensity(0, 10);

    //         for (int bar = 0; bar < width / segmentSize; bar++)
    //         {
    //             for (int i = 0; i < segmentSize; i++)
    //             {
    //                 float fadeValue = softEdge(i - baseSize, size * lfo.getValue(2. * float(bar) / (width / segmentSize) * offset) * segmentSize);
    //                 int index = bar * segmentSize + i;
    //                 if (remap) index = remap[index];
    //                 pixels[index] = RGBA(0,0,0,255) * (1.-fadeValue) * transition.getValue();
    //             }
    //         }
    //     }
    // };

    // class RotatingRingsMaskPattern : public Pattern<RGBA>
    // {
    //     Transition transition;
    //     PixelMap3d *map;
    //     LFO<Sin> ring1;
    //     LFO<Sin> ring2;

    // public:
    //     RotatingRingsMaskPattern(PixelMap3d *map)
    //     {
    //         this->map = map;
    //         this->name = "Rotating rings mask";
    //     }

    //     inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    //     {
    //         if (!transition.Calculate(active))
    //             return; // the fade out is done. we can skip calculating pattern data

    //         auto col1 = params->getPrimaryColour() * transition.getValue();
    //         auto col2 = params->getSecondaryColour() * transition.getValue();
    //         float size = params->getSize(0.01, 0.1);
    //         float zoffset = params->getVariant(0, -0.2);
    //         ring1.setPeriod(params->getVelocity(20000, 2000));
    //         ring2.setPeriod(params->getVelocity(14000, 1400));

    //         for (int index = 0; index < width; index++)
    //         {
    //             float z_norm = fromBottom(map->y(index));
    //             float offset = (map->z(index) + 2) / 2 * params->getOffset();

    //             pixels[index] = RGBA(0,0,0,255) * (
    //                 1.
    //                 -softEdge(abs(z_norm - ring1.getValue(offset)), size) 
    //                 -softEdge(abs(z_norm - ring2.getValue(offset)), size)
    //             ) * transition.getValue();
    //         }
    //     }
    // };

};