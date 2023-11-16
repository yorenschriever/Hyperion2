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
#include "../midiController.hpp"
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

    class WindowMapPattern : public Pattern<RGBA>
    {
        const int estimate[6] = {     
            66,   //boogje  rechts
            172,  //rechts naar beneden
            112,  //horizontaal
            172,  //links naar boven
            66,   //boogje links
            172,  //midden naar beneden
        };

        RGBA colors[7] = {
            RGBA(255, 0, 0, 255),
            RGBA(0, 255, 0, 255),
            RGBA(0, 0, 255, 255),
            RGBA(0, 255, 255, 255),
            RGBA(255, 0, 255, 255),
            RGBA(255, 255, 0, 255),
            RGBA(255, 255, 255, 255),
        };

    public:
        WindowMapPattern(uint16_t *remap=nullptr)
        {
            this->name = "Window mapper";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            int nextColorAt;
            int segmentIndex;
            for (int i = 0; i < width; i++)
            {
                int ii = i % (width/8);
                if (ii==0){
                    segmentIndex=-1;
                    nextColorAt=0;
                }
                if (ii==nextColorAt){
                    segmentIndex++;
                    nextColorAt += estimate[segmentIndex] + Corrections::corrections[segmentIndex];
                }
    
                pixels[i] = colors[segmentIndex];
            }
        }
    };

class Gamma : public Pattern<RGBA>
    {
    public:
        int gradientSize=10;
        Gamma(int gradientSize)
        {
            this->gradientSize = gradientSize;
            this->name = "Gamma";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            float gammaCorrection = params->getAmount(0.5,4);
            int dimR = params->getOffset(10,255);
            int dimG = params->getVariant(10,255);
            int dimB = params->getIntensity(10,255);

            Log::info("Gamma", "Correction parameters: gamma=%f, \tR=%d, \tG=%d, \tB=%d\t(remove existing correction first)", gammaCorrection, dimR, dimG, dimB);

            for (int i = 0; i < width; i++)
            {
                int pos = (i % gradientSize) * 255 / gradientSize;
                pixels[i] = RGB(
                    pow((float)pos*dimR / 255. / 255., gammaCorrection) * 255,
                    pow((float)pos*dimG / 255. / 255., gammaCorrection) * 255,
                    pow((float)pos*dimB / 255. / 255., gammaCorrection) * 255
                );
      
            }
        }
    };

}