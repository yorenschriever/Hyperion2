#include "spi.hpp"
#include "log.hpp"
#include "spi-esp32.hpp"

const int num_hosts = 2;
spi_host_device_t hosts[num_hosts] = {HSPI_HOST, VSPI_HOST};
int hostIndex=0;

SPI* SPIFactory::createInstance()
{
    if (hostIndex < num_hosts)
        return new SPI_ESP(hosts[hostIndex++]);

    Log::error("spi","Maximum number of %d spi outputs is reached. This spi instance will not output data",num_hosts);
    return nullptr;
}