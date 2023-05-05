#include "rmt.hpp"
#include "log.hpp"


void RMT::begin(unsigned int port)
{
    Log::error("RMT","RMT not supported on this platform. Neopixel data will not be sent out");
}

void RMT::send(uint8_t *buffer, int length)
{
}

bool RMT::ready()
{
    return false;
}