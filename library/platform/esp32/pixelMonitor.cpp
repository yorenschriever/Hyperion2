#include "pixelMonitor.hpp"
#include "platform/includes/log.hpp"

static const char *TAG = "PIXEL_MONITOR";

unsigned int PixelMonitor::addOutput(PixelMap map)
{
    Log::error(TAG,"PixelMonitor not implemented for this platform");
    return 0;
}

void PixelMonitor::begin()
{

}
