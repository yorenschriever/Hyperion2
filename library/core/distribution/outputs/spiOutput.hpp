#pragma once

#include <algorithm>
#include "baseOutput.hpp"
#include "thread.hpp"
#include "utils.hpp"
#include "log.hpp"
#include "spi.hpp"

// SpiOutput sends the output over spi.
// Can be used for pixel led strands.
class SpiOutput final: public BaseOutput
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


    bool ready() override
    {
        // even though the datasheet states less, 1000us seems to be necessary for
        // stable operation (tested on ledster)
        return spi && spi->ready();
    }


    void initialize() override
    {
        if (spi)
            spi->begin(clkPin, dataPin, frq );
    }

    void process(const Buffer& inputBuffer) override
    {
        if (!ready())
            return;

        if (!inputBuffer.size())
            return;

        if (inputBuffer.size() != length){
            buffer = (uint8_t *)Utils::realloc_dma(buffer, inputBuffer.size());
            if (!buffer)
            {
                Log::error("SPI_OUTPUT", "Unable to allocate memory for SPIOutput, free heap = %d", Utils::get_free_heap());
                Utils::exit();
            }
            length = inputBuffer.size();
            memset(buffer, 0, length);
        }

        memcpy(this->buffer, inputBuffer.data(), length);

        spi->send(buffer, length);

        fpsCounter.increaseUsedFrameCount();
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
