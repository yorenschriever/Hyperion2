#pragma once
#include "../mapping/ceilingMappedIndices.hpp"
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

    class ShowStarts : public Pattern<RGBA>
    {
    public:
        int segmentSize;
        int colorChange;
        ShowStarts(int colorChange = 8, int segmentSize=60)
        {
            this->segmentSize = segmentSize;
            this->colorChange = colorChange;

            this->name = "Mapping starts";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            RGBA colors[6] = {
                RGBA(255, 0, 0, 255),
                RGBA(0, 255, 0, 255),
                RGBA(0, 0, 255, 255),
                RGBA(0, 255, 255, 255),
                RGBA(255, 0, 255, 255),
                RGBA(255, 255, 0, 255),
            };

            for (int i = 0; i < width; i++)
            {
                int segmentIndex = i%segmentSize;
                int segment = i/segmentSize;

                int colorIndex = (segment/colorChange) % 6;

                if (i % segmentSize < 20)
                    pixels[i] = colors[colorIndex] * (1. - (((float)(i % segmentSize)) / 20.));
            }
        }
    };

    class OneColor : public Pattern<RGBA>
    {
    public:
        RGBA color;
        OneColor(RGBA color, const char *name)
        {
            this->color = color;
            this->name = name;
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            for (int i = 0; i < width; i++)
            {
                pixels[i] = color;
            }
        }
    };


    class PaletteColors : public Pattern<RGBA>
    {
    public:
        PaletteColors()
        {
            this->name = "Palette colors";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            for (int i = 0; i < width; i++)
                pixels[i] = i<width/2 ? params->getPrimaryColour() : params->getSecondaryColour();
        }
    };

    class PaletteGradient : public Pattern<RGBA>
    {
    public:
        uint16_t *remap;
        PaletteGradient(uint16_t *remap=nullptr)
        {
            this->name = "Palette gradient";
            this->remap=remap;
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            for (int i = 0; i < width; i++)
            {
                int gradientPos = 2 * i * 255 / width;
                int ii = remap ? remap[i] : i;
                pixels[ii] = params->getGradient(gradientPos % 255);
            }
        }
    };

}