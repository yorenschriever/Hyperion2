#pragma once
#include "core/generation/patterns/pattern.hpp"
#include "platform/includes/artNet.hpp"
#include <math.h>
#include <vector>

namespace Patterns
{

    class ArtNetPattern : public Pattern<RGBA>
    {
        int startChannel;
        int sequenceNumber = 0;
        int segmentSize;
        uint16_t *remap;
        ArtNetUniverse *universe = nullptr;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        ArtNetPattern(uint16_t port = 0, int startChannel=0, int segmentSize=60, uint16_t *remap=nullptr)
        {
            this->name = "ArtNet primary color";
            auto artnet = ArtNet::getInstance();
            universe = artnet->addUniverse(port);
            this->startChannel = startChannel;
            this->segmentSize = segmentSize;
            this->remap=remap;
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            for (int segment = 0; segment < width/segmentSize; segment++)
            {
                if(segment+startChannel >= universe->size) 
                    break;

                auto col = params->getPrimaryColour() * (float(universe->channels[segment+startChannel]/255.)) * transition.getValue();
                for(int i=0;i<segmentSize;i++)
                {
                    if (remap)
                        pixels[remap[segment*segmentSize+i]] = col;
                    else
                        pixels[segment*segmentSize+i] = col;
                }
            }
        }
    };

}