#include "spi.hpp"
#include "spi-esp32.hpp"

SPI* SPIFactory::createInstance()
{
    return new SPI_ESP();
}