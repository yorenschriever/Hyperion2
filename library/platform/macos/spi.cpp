#include "spi.hpp"
#include "log.hpp"

SPI* SPIFactory::createInstance()
{
    Log::error("SPI","SPI not supported on this platform. SPI data will not be sent out");
    return nullptr;
}