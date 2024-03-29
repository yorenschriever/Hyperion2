#pragma once

#include "../inputs/input.hpp"
#include "../luts/lut.hpp"
#include "../outputs/output.hpp"
#include "platform/includes/log.hpp"
#include <inttypes.h>

// Pipes are objects that tie an input and output together.
// The input provides the data and the output sends it out to the lamps.
class Pipe
{
public:
#if (ESP_PLATFORM)
    static const int bufferSize = 1200*4;
#else
    static const int bufferSize = 1500 * 4 * 6;
#endif
private:
    static uint8_t buffer[bufferSize];

    virtual int transfer(uint8_t *data, int length, Output *out)
    {
        out->setLength(length);
        out->setData(data, length, 0);
        return length; // unkown colour format, assume 1 pixel per byte
    }

public:
    Pipe(Input *in, Output *out)
    {
        this->in = in;
        this->out = out;
    }
    Output *out;
    Input *in;

    inline void process()
    {
        if (!out->ready())
            return;

        int datalength = in->loadData(buffer, bufferSize);
        if (datalength == 0)
            return;

        // Log::info("Pipe","data %s LENGTH = %d", buffer , datalength);

        numPixels = this->transfer(buffer, datalength, out);

        out->show();
    }

    int getNumPixels()
    {
        return numPixels;
    }

private:
    int numPixels = 0; // keep track of the number of pixels is for stats only.
};

uint8_t Pipe::buffer[bufferSize];