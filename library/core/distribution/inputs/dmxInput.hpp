#pragma once

#include <algorithm>
#include <cstring>
#include "input.hpp"
#include "dmx.hpp"

//DMXInput listens to input on the specified dmx port.
class DMXInput : public Input
{

public:
    //startChannel = the first channel it will listen to (1-512)
    //length = the number of channels is will read
    DMXInput(int startChannel = 1, int length = 512, int dmxPort = 0)
    {
        this->startChannel = std::min(512, std::max(1, startChannel));
        this->length = std::min(513 - this->startChannel, length);
        this->dmxPort = dmxPort;
    }

    virtual void begin() override
    {
        this->dmx = DMX::getInstance(dmxPort);
    }

    virtual int loadData(uint8_t *dataPtr, unsigned int buffersize) override
    {
        int frameNumber = dmx->getFrameNumber();

        //check if there is a new frame since last time
        if (frameNumber == lastFrameNumber)
            return 0;

        usedframecount++;
        missedframecount += frameNumber - lastFrameNumber - 1;
        lastFrameNumber = frameNumber;

        memcpy(dataPtr, dmx->getDataPtr() + startChannel, std::min(length,(int) buffersize));

        return length;
    }

private:
    DMX *dmx = nullptr;
    int dmxPort;

    int startChannel;
    int length;
    unsigned int lastFrameNumber = 0;
};