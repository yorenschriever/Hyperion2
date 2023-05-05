#include "pixelMonitor3d.hpp"
#include "platform/includes/log.hpp"
#include "thread.hpp"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

static const char *TAG = "PIXEL_MONITOR_3D";

unsigned int PixelMonitor3d::addOutput(PixelMap3d map)
{
    return 0;
}

void PixelMonitor3d::begin()
{
    Log::error(TAG, "PixelMonitor3d not supported on this platform");
}

void PixelMonitor3d::monitorTask(void *pvParameters)
{
}

std::vector<PixelMonitor3d::PixelMonitorOutput3d> PixelMonitor3d::outputs;
bool PixelMonitor3d::begun = false;
unsigned int PixelMonitor3d::port = 9750;