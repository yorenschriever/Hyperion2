#pragma once

#include "slicer.hpp"

// Splitter is a receiver that splits the itself into separate senders that can be used to route a parts of the data to separate chains.
class Splitter : public Slicer
{

public:
    /*
     * - length is in bytes, not in lights
     * - if sync is on, a new frame will not be loaded until all outputs have finished sending their data.
     * This is useful when the sourceInput is an input that generates data on demand instead of listening
     * to incoming data. Without sync it would generate a new frame when any of the destination outputs
     * requests new data.
     * If sync is false you basically sync to the input.
     */
    Splitter(std::vector<int> lengths, bool sync = false) : Slicer({})
    {
        int start = 0;
        for (int length : lengths)
        {
            auto bi = new BufferInput(this->buffer + start, length);
            this->destinationInputs.push_back(bi);
            this->slices.push_back({start, length, sync});
            start += length;

            bufferSize = std::max(bufferSize, start + length);
        }

        buffer = (uint8_t *)malloc(bufferSize);
        if (!buffer)
        {
            Log::error("Splitter", "Unable to allocate buffer for splitter.");
        }
    }
};