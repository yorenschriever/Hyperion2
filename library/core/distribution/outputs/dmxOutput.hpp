#pragma once

#include "dmx.hpp"
#include "baseOutput.hpp"

// DMXOutput writes led data to the dmx output. You can use multiple dmx outputs
// with different start channels. Their data will be combined into a single dmx frame
// before it is sent out.
class DMXOutput : public BaseOutput
{
public:
    DMXOutput(int dmxPort = 0)
    {
        this->dmxPort = dmxPort;
    }

    // size is in bytes
    void setData(uint8_t *data, int size) override
    {
        if (dmx)
            dmx->write(data, size, 1);
    }

    bool ready() override
    {
        return dmx && dmx->ready();
    }

    void show() override
    {
        dmx->show();
    }

    void begin() override
    {
        this->dmx = DMX::getInstance(dmxPort);
    }

    void clear() override
    {
        if (dmx)
            dmx->clearTxBuffer();
    }

    // length is in bytes
    void setLength(int len) override
    {
        // do nothing. DMX automatically detects the length based on the last byte written
    }

private:
    unsigned int dmxPort;
    DMX *dmx = nullptr;
};