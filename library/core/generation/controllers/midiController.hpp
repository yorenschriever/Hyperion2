#pragma once
#include "midi-listener.hpp"
#include "../controlHub/IHubController.hpp"

class MidiController : public IMidiListener, public IHubController
{
public: 
    virtual ~MidiController() = default;

protected:
    inline uint8_t scale127to255(uint8_t midiValue){
        //one extra step needs to be inserted somewhere, because 2*127 = 254 and we want to reach 255. 
        //this could be in the middle, but i decided to do it right before the highest value.
        if (midiValue == 127) return 255;
        return midiValue * 2;
    }
};