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

    Buffer *getData() override 
    {
        int frameNumber = dmx->getFrameNumber();

        //check if there is a new frame since last time
        if (frameNumber == lastFrameNumber)
            return nullptr;
        lastFrameNumber = frameNumber;

        fpsCounter.increaseUsedFrameCount();
        fpsCounter.increaseMissedFrameCount(frameNumber - lastFrameNumber - 1);

        auto patternBuffer = BufferPool::getBuffer(length);
        if (!patternBuffer)
        {
            Log::error("DMX_INPUT", "Unable to allocate memory for PatternInput, free heap = %d\n", Utils::get_free_heap());
            Utils::exit();
        }
        auto dataPtr = patternBuffer->getData();

        memcpy(dataPtr, dmx->getDataPtr() + startChannel, length);

        return patternBuffer;
    }

private:
    DMX *dmx = nullptr;
    int dmxPort;

    int startChannel;
    int length;
    unsigned int lastFrameNumber = 0;
};