#pragma once
#include "core/generation/patterns/pattern.hpp"
#include "generation/patterns/helpers/fade.h"
#include "generation/patterns/helpers/interval.h"
#include "generation/patterns/helpers/timeline.h"
#include "generation/pixelMap.hpp"
#include <math.h>
#include <vector>
#include "../mapping/ceilingMappedIndices.hpp"

namespace Patterns
{

    class CeilingChase : public Pattern<RGBA>
    {
        int segmentSize;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<SawDown> lfo;

    public:
        CeilingChase(int segmentSize=60)
        {
            this->segmentSize = segmentSize;
            this->name = "Ceiling chase";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(params->getVelocity(5000,500));
            int multiply = int(params->getAmount(1,10));
            lfo.setDutyCycle(params->getSize());
            lfo.setSoftEdgeWidth(1./width);

            for (int i=0;i<width; i++)
                pixels[ceilingMappedIndices[i]] = params->getPrimaryColour() * lfo.getValue(float(i)/width * multiply) * transition.getValue();

        }
    };

};