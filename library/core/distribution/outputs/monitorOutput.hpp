#pragma once

#include "output.hpp"
#include "udpOutput.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/pixelMonitor.hpp"
#include "core/generation/pixelMap.hpp"

// MonitorOutput displays the led data on your monitor.
// It starts a python script that can render udp data, and
// sends the data using UDPOutput
class MonitorOutput : public UDPOutput
{
public:
    MonitorOutput(PixelMap map, unsigned int fps = 60) : UDPOutput("localhost",0, fps)
    {
        this->map = map;
        this->port = PixelMonitor::addOutput(map);
    }

    void begin() override
    {
        UDPOutput::begin();
        PixelMonitor::begin();
    }

private:
    PixelMap map;
    PixelMonitor monitor;
};