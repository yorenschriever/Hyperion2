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
        int segmentSize;
        ShowStarts(int segmentSize){
            this->segmentSize = segmentSize;
            this->name = "Mapping starts";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            RGBA colors[6] = {
                RGBA(255,0,0,  255),
                RGBA(0,255,0,  255),
                RGBA(0,0,255,  255),
                RGBA(0,255,255,255),
                RGBA(255,0,255,255),
                RGBA(255,255,0,255),
            };

            for(int i=0;i<width;i++){
                if (i%segmentSize < 20)
                    pixels[i] = colors[i/480] * (1.-(((float)(i%segmentSize))/20.));
            }
        }
    };

}