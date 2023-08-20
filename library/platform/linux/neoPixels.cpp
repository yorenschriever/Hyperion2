#include "neoPixels.hpp"
#include "log.hpp"

NeoPixels *NeoPixels::createInstance()
{
    Log::error("RMT","RMT not supported on this platform. Neopixel data will not be sent out");
    return nullptr;
}
