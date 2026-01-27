#pragma once

#include "log.hpp"
#include "neoPixels.hpp"
#include "baseOutput.hpp"
#include "thread.hpp"
#include "utils.hpp"
#include <algorithm>

// Writes the led data to the octows2811 style connector on the front panel.
// It use esp32's RMT peripheral to do this.
class NeopixelOutput final: public BaseOutput
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


    bool ready() override
    {
        return rmt && rmt->ready();
    }


    void initialize() override
    {
        if (!rmt)
            return;

        rmt->begin(port, timing);
    }

    void clear() override
    {
        memset(buffer, 0, length);
    }

    void process(Buffer inputBuffer) override
    {
        if (!ready())
            return;

        if (!inputBuffer.size())
            return;

        if (inputBuffer.size() != length){
            buffer = (uint8_t *)realloc(buffer, inputBuffer.size());
            if (!buffer)
            {
                Log::error("NEOPIXEL_OUTPUT", "Unable to allocate memory for neoPixelOutput, free heap = %d", Utils::get_free_heap());
                Utils::exit();
            }
            length = inputBuffer.size();
            memset(buffer, 0, length);
        }

        memcpy(this->buffer, inputBuffer.data(), length);

        rmt->send(buffer, length);

        fpsCounter.increaseUsedFrameCount();
    }

private:
    uint8_t *buffer;
    int port = 1;
    NeoPixels *rmt;
    int length;
    NeoPixels::Timing timing;
};