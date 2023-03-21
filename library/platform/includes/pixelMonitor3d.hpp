#pragma once

#include <stdint.h>
#include <vector>
#include "core/generation/pixelMap.hpp"

class PixelMonitor3d
{
public:
    static unsigned int addOutput(PixelMap3d map);
    static void begin();
private: 

    struct PixelMonitorOutput3d
    {
        PixelMap3d map;
        unsigned int port;
    };

    static void monitorTask(void * pvParameters);
    static std::vector<PixelMonitorOutput3d> outputs;
    static bool begun;
    static unsigned int port;
};