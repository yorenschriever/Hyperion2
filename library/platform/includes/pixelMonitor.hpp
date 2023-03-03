#pragma once

#include <stdint.h>
#include "core/generation/pixelMap.h"

class PixelMonitor
{
public:
    void initialize(PixelMap map, unsigned int port);
private: 
    static void monitorTask(void * pvParameters);
};