#pragma once

#include <algorithm>
#include <cstring>
#include "baseInput.hpp"
#include "dmx.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/utils.hpp"

//DMXInput listens to input on the specified dmx port.
class DMXInput final: public BaseInput
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

    void initialize() override
    {
        this->dmx = DMX::getInstance(dmxPort);
    }

    virtual bool ready() override
    {
        if(!dmx || !dmx->isHealthy()) 
            return false;

        int frameNumber = dmx->getFrameNumber();
        return (frameNumber != lastFrameNumber);
    }

    Buffer process() override 
    {
        if (!ready())
            return Buffer(0);

        int frameNumber = dmx->getFrameNumber();
        lastFrameNumber = frameNumber;

        fpsCounter.increaseUsedFrameCount();
        fpsCounter.increaseMissedFrameCount(frameNumber - lastFrameNumber - 1);

        auto patternBuffer = Buffer(length);

        memcpy(patternBuffer.data(), dmx->getDataPtr() + startChannel, length);

        return patternBuffer;
    }

private:
    DMX *dmx = nullptr;
    int dmxPort;

    int startChannel;
    int length;
    unsigned int lastFrameNumber = 0;
};