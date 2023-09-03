#pragma once

#include "dmx.hpp"
#include "output.hpp"

// DMXOutput writes led data to the dmx output. You can use multiple dmx outputs
// with different start channels. Their data will be combined into a single dmx frame
// before it is sent out.
class DMXOutput : public Output
{
public:
    // start channel 1-512
    DMXOutput(int startChannel, int dmxPort = 0)
    {
        if (startChannel < 1)
            startChannel = 1;
        if (startChannel > 512)
            startChannel = 512;
        this->startChannel = startChannel;
        this->dmxPort = dmxPort;
    }

    // index and size are in bytes
    void setData(uint8_t *data, int size, int index) override
    {
        if (dmx)
            dmx->write(data, size, index + startChannel);
    }

    bool ready() override
    {
        return dmx && dmx->ready();
    }

    void show() override
    {
        // set this static variable to indicate that any dmx instance has updated the data
        groupDirty = true;
    }

    void postProcess() override
    {
        // read the static variable to see if any instance has updated the data
        if (!groupDirty)
            return;

        if (dmx)
            dmx->show();
        groupDirty = false;
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
    int startChannel;
    unsigned int dmxPort;
    DMX *dmx = nullptr;

    static bool groupDirty;
};

bool DMXOutput::groupDirty = false;