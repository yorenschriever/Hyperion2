#pragma once
#include "core/generation/patterns/pattern.hpp"
#include "generation/patterns/helpers/fade.h"
#include "generation/patterns/helpers/interval.h"
#include "generation/patterns/helpers/timeline.h"
#include "generation/pixelMap.hpp"
#include "../distributeAndMonitor.hpp"
#include "mappingHelpers.hpp"
#include <math.h>
#include <vector>

namespace TestPatterns
{

    class ShowStarts : public Pattern<RGBA>
    {
    public:
        int barSize;
        int barChainSize;
        int fadeSize;
        static const int numColors = 13;
        RGB colors[numColors] = {
            0xFF0000,
            0x00FF00,
            0x0000FF,
            0x00FFFF,
            0xFF00FF,
            0xFFFF00,
            0xFFFFFF,
            
            0xFF7F00,
            0xFF007F,
            0x7FFF00,
            0x7F00FF,
            0x00FF7F,
            0x007FFF,
        };
        ShowStarts(int barSize=60, int barChainSize=8, int fadeSize=20)
        {
            this->barSize = barSize;
            this->barChainSize = barChainSize;
            this->fadeSize = fadeSize;
            this->name = "Mapping starts";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            for (int i = 0; i < width; i++)
            {
                if (i % barSize < fadeSize)
                    pixels[i] = (RGBA)colors[(i / (barSize*barChainSize)) % numColors] * (1. - (((float)(i % barSize)) / float(fadeSize)));
            }
        }
    };

class DistributionPattern : public Pattern<RGBA>
    {
    public:
        int barSize;
        int fadeSize;
        std::vector<int> sizes;
        static const int numColors = 13;
        RGB colors[numColors] = {
            0xFF0000,
            0x00FF00,
            0x0000FF,
            0x00FFFF,
            0xFF00FF,
            0xFFFF00,
            0xFFFFFF,
            
            0xFF7F00,
            0xFF007F,
            0x7FFF00,
            0x7F00FF,
            0x00FF7F,
            0x007FFF,
        };
        DistributionPattern(Distribution distribution, int barSize=60, int fadeSize=20)
        {
            this->barSize = barSize;
            this->fadeSize = fadeSize;
            this->name = "Distribution";

            std::transform(distribution.begin(), distribution.end(), std::back_inserter(sizes), 
                [](Slave slave) -> int{ return slave.size; });
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            int start = 0;
            int colIndex = 0;
            for (auto size : sizes)
            {
                RGBA color = colors[colIndex % numColors];
                for (int i = 0; i < size; i++)
                {
                    int index = i+start;
                    if (index > width)
                        break;
                    if (i % barSize < fadeSize)
                        pixels[index] = color * (1. - (((float)(i % barSize)) / float(fadeSize)));
                }

                start += size;
                colIndex++;
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
                    pixels[i] = params->getGradient(gradientPos);
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

}