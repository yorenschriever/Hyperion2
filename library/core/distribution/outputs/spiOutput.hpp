#pragma once

#include <algorithm>
#include "baseOutput.hpp"
#include "thread.hpp"
#include "utils.hpp"
#include "log.hpp"
#include "spi.hpp"

// SpiOutput sends the output over spi.
// Can be used for pixel led strands.
class SpiOutput : public BaseOutput
{
public:
    SpiOutput(uint8_t clkPin, uint8_t dataPin, int frq = 500000)
    {
        spi = SPI::createInstance();
        this->clkPin = clkPin;
        this->dataPin = dataPin;
        this->frq = frq;

        this->length = 12;
        buffer = (uint8_t *)Utils::malloc_dma(length);
    }

    // index and size are in bytes
    void setData(uint8_t *data, int size) override
    {
        int copylength = std::min(size, length);
        if (copylength > 0)
            memcpy(this->buffer, data, copylength);
    }

    bool ready() override
    {
        // even though the datasheet states less, 1000us seems to be necessary for
        // stable operation (tested on ledster)
        return spi && spi->ready();
    }

    void show() override
    {
        spi->send(buffer, length);
    }

    void begin() override
    {
        if (spi)
            spi->begin(clkPin, dataPin, frq );
    }

    void clear() override
    {
        memset(buffer, 0, length);
    }

    // length is in bytes
    void setLength(int len) override
    {
        if (len != this->length)
        {
            // wait for the buffer to be sent before we are going to change its size
            while (!ready())
                Thread::sleep(1);

            buffer = (uint8_t *)Utils::realloc_dma(buffer, len);

            if (!buffer)
            {
                Log::error("SPI","Unable to allocate memory for SPIOutput, free heap = %d",Utils::get_free_heap());
                Utils::exit();
            }

            memset(buffer, 0, len);
            this->length = len;
        }
    }

    ~SpiOutput()
    {
        //Utils::free_dma(buffer); TODO
        if (spi)
            delete spi;
    }
private:
    SPI *spi;
    uint8_t clkPin;
    uint8_t dataPin;
    int frq;

    int length;
    uint8_t *buffer;
};
