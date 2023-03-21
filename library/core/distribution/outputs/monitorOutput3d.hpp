#pragma once

#include "output.hpp"
#include "udpOutput.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/pixelMonitor3d.hpp"
#include "core/generation/pixelMap.hpp"

// MonitorOutput displays the led data on your monitor.
// It starts a python script that can render udp data, and
// sends the data using UDPOutput
class MonitorOutput3d : public UDPOutput
{
public:
    MonitorOutput3d(PixelMap3d map, unsigned int fps = 60) : UDPOutput("localhost",0, fps)
    {
        this->map = map;
        this->port = PixelMonitor3d::addOutput(map);
    }

    void begin() override
    {
        UDPOutput::begin();
        PixelMonitor3d::begin();
    }

private:
    PixelMap3d map;
    PixelMonitor3d monitor;
};