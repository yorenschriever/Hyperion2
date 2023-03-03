#pragma once

#include "output.hpp"
#include "udpOutput.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/pixelMonitor.hpp"
#include "core/generation/pixelMap.h"

// MonitorOutput displays the led data on your monitor.
// It starts a python script that can render udp data, and
// sends the data using UDPOutput
class MonitorOutput : public Output
{
public:
    MonitorOutput(PixelMap map, unsigned int fps = 60)
    {
        this->map = map;
        //todo dynamically find free port
        this->udpOutput = new UDPOutput("localhost",9699, fps);
    }

    // index and size are in bytes
    void setData(uint8_t *data, int size, int index) override
    {
        udpOutput->setData(data,size,index);
    }

    bool ready() override
    {
        return udpOutput->ready();
    }

    void show() override
    {
        udpOutput->show();
    }

    void begin() override
    {
        udpOutput->begin();
        monitor.initialize(map, 9699);
    }

    void clear() override
    {
        udpOutput->clear();
    }

    // length is in bytes
    void setLength(int len) override
    {
        udpOutput->setLength(len);
    }

    ~MonitorOutput(){
        delete udpOutput;
    }

private:
    PixelMap map;
    UDPOutput *udpOutput;
    PixelMonitor monitor;
};