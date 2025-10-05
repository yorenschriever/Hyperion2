#pragma once

#include "log.hpp"
#include "neoPixels.hpp"
#include "baseOutput.hpp"
#include "thread.hpp"
#include "utils.hpp"
#include <algorithm>

// Writes the led data to the octows2811 style connector on the front panel.
// It use esp32's RMT peripheral to do this.
class NeopixelOutput : public BaseOutput
{
public:
    // port goes from 1-8
    NeopixelOutput(int port, NeoPixels::Timing timing = NeoPixels::Kpbs800)
    {
        this->port = port;
        this->timing = timing;

        // initialize with 3 pixels by default
        this->length = 12;
        buffer = (uint8_t *)malloc(length);

        rmt = NeoPixels::createInstance();
    }

    ~NeopixelOutput(){
        if (rmt) 
            delete rmt;
    }

    // index and size are in bytes
    void setData(uint8_t *data, int size) override
    {
        int copy_length = std::min(size, length);
        if (copy_length > 0)
            memcpy(this->buffer, data, copy_length);
    }

    bool ready() override
    {
        return rmt && rmt->ready();
    }

    void show() override
    {
        if (!ready())
            return;

        rmt->send(buffer, length);
    }

    void begin() override
    {
        if (!rmt)
            return;

        rmt->begin(port, timing);
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

            buffer = (uint8_t *)realloc(buffer, len);
            if (!buffer)
            {
                Log::error("NEOPIXEL_OUTPUT", "Unable to allocate memory for neoPixelOutput, free heap = %d", Utils::get_free_heap());
                Utils::exit();
            }

            memset(buffer, 0, len);
            this->length = len;
        }
    }

private:
    uint8_t *buffer;
    int port = 1;
    NeoPixels *rmt;
    int length;
    NeoPixels::Timing timing;
};