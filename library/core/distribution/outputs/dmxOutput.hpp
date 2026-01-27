#pragma once

#include "dmx.hpp"
#include "baseOutput.hpp"

// DMXOutput writes led data to the dmx output. You can use multiple dmx outputs
// with different start channels. Their data will be combined into a single dmx frame
// before it is sent out.
class DMXOutput final: public BaseOutput
{
public:
    DMXOutput(int dmxPort = 0)
    {
        this->dmxPort = dmxPort;
    }


    bool ready() override
    {
        return dmx && dmx->ready();
    }


    void initialize() override
    {
        this->dmx = DMX::getInstance(dmxPort);
    }

    void clear() override
    {
        if (dmx)
            dmx->clearTxBuffer();
    }

    void process(Buffer inputBuffer) override
    {
        if (!ready())
            return;

        dmx->write(inputBuffer.data(), inputBuffer.size(), 1);
        dmx->show();

        fpsCounter.increaseUsedFrameCount();
    }

private:
    unsigned int dmxPort;
    DMX *dmx = nullptr;
};