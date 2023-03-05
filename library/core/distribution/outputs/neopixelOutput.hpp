#pragma once 

#include <algorithm>
#include "output.hpp"
#include "rmt.hpp"
#include "thread.hpp"
#include "utils.hpp"
#include "log.hpp"

//Writes the led data to the octows2811 style connector on the front panel.
//It use esp32's RMT peripheral to do this.
class NeopixelOutput : public Output
{
public:
    //port goes from 1-8
    NeopixelOutput(int port)
    {
        this->port = port;

        //initialize with 3 pixels by default
        this->length = 12;
        buffer = (uint8_t *)Utils::malloc_dma(length);
    }

    //index and size are in bytes
    void setData(uint8_t *data, int size, int index) override
    {
        int copy_length = std::min(size, length - index);
        if (copy_length > 0)
            memcpy(this->buffer + index, data, copy_length);
    }

    bool ready() override
    {
        return rmt.ready();
    }

    void show() override
    {
        if (!ready())
            return;

        rmt.send(buffer,length);
    }

    void begin() override
    {
        rmt.begin(port);
    }

    void clear() override
    {
        memset(buffer, 0, length);
    }

    //length is in bytes
    void setLength(int len) override
    {
        if (len != this->length)
        {
            //wait for the buffer to be sent before we are going to change its size
            while (!ready())
                Thread::sleep(1);

            buffer = (uint8_t *)Utils::realloc_dma(buffer, len);

            if (!buffer)
            {
                Log::error("NEOPIXEL_OUTPUT","Unable to allocate memory for neoPixelOutput, free heap = %d",Utils::get_free_heap());
                Utils::exit();
            }

            memset(buffer, 0, len);
            this->length = len;
        }
    }

private:
    uint8_t *buffer;
    int port = 1;
    RMT rmt;
    int length;
};