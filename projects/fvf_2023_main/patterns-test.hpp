#pragma once
#include "core/generation/patterns/pattern.hpp"
#include "generation/patterns/helpers/fade.h"
#include "generation/patterns/helpers/interval.h"
#include "generation/patterns/helpers/timeline.h"
#include "generation/pixelMap.hpp"
#include "mappingHelpers.hpp"
#include <math.h>
#include <vector>

namespace TestPatterns
{

    class ShowStarts : public Pattern<RGBA>
    {
    public:
        int segmentSize=1;
        ShowStarts(int segmentSize=1)
        {
            this->segmentSize = segmentSize;
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
                if (i % segmentSize < 20)
                    pixels[i] = colors[i / 480] * (1. - (((float)(i % segmentSize)) / 20.));
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



    class Palette : public Pattern<RGBA>
    {
    public:
        int gradientSize=10;
        int staticColorSize=10;
        Palette(int gradientSize, int staticColorSize)
        {
            this->gradientSize = gradientSize;
            this->staticColorSize = staticColorSize;
            this->name = "Palette";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            int totalSize = gradientSize + 3*staticColorSize;

            for (int i = 0; i < width; i++)
            {
                int pos = i % totalSize;
                if (pos < gradientSize){
                    int gradientPos = pos * 255 / gradientSize;
                    pixels[i] = params->gradient->get(gradientPos);
                    continue;
                } 

                pos = pos - gradientSize;
                if (pos < staticColorSize){
                    pixels[i] = params->getPrimaryColour();
                } else if (pos < 2 * staticColorSize){
                    pixels[i] = params->getSecondaryColour();
                } else {
                    pixels[i] = params->getHighlightColour();
                } 
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


    class BrightnessMatch : public Pattern<RGBA>
    {
    public:
        BrightnessMatch()
        {
            this->name = "Brightness match";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            int dimLedster = params->getOffset(0,255);
            int dimStokken = params->getVariant(0,255);
            int dimHalo = params->getIntensity(0,255);

            RGB color;
            if (width==481) color = Monochrome(dimLedster);
            else if (width==96) color = Monochrome(dimHalo);
            else color = Monochrome(dimStokken);

            Log::info("BrightnessMatch", "Brightness Correction parameters:\tLedster=%d, \tStokken=%d, \tHalo=%d\t(remove existing correction first)", dimLedster, dimStokken, dimHalo);

            for (int i = 0; i < width; i++)
                pixels[i] = color;
        }
    };
}