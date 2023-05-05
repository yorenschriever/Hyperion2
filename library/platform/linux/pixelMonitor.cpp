#include "pixelMonitor.hpp"
#include "platform/includes/log.hpp"
#include "thread.hpp"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

static const char *TAG = "PIXEL_MONITOR";

unsigned int PixelMonitor::addOutput(PixelMap map)
{
    return 0;
}

void PixelMonitor::begin()
{
    Log::error(TAG, "PixelMonitor3d not supported on this platform");
}

void PixelMonitor::monitorTask(void *pvParameters)
{
}

std::vector<PixelMonitor::PixelMonitorOutput> PixelMonitor::outputs;
bool PixelMonitor::begun = false;
unsigned int PixelMonitor::port = 9700;