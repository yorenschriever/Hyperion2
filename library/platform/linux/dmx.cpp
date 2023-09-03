#include "dmx.hpp"
#include "log.hpp"

DMX* DMX::getInstance(unsigned int port)
{
    Log::error("DMX","DMX not supported on this platform. DMX data will not be sent /received");
    return nullptr;
}