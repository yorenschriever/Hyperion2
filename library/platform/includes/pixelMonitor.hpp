#pragma once

#include <stdint.h>
#include <vector>
#include "core/generation/pixelMap.hpp"

class PixelMonitor
{
public:
    static unsigned int addOutput(PixelMap map);
    static void begin();
private: 

    struct PixelMonitorOutput
    {
        PixelMap map;
        unsigned int port;
    };

    static void monitorTask(void * pvParameters);
    static std::vector<PixelMonitorOutput> outputs;
    static bool begun;
    static unsigned int port;
};