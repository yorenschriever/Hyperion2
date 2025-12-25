#pragma once

#include "../inputs/bufferInput.hpp"
#include "../interfaces.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cstring>
#include <vector>

// InputSlicer is a more advanced version of input splitter.
// With this class you can freely assign start and end positions
// of each slice. This means you can also skip parts of the buffer,
// or use parts twice.
class Slicer : public IReceiver
{

public:
    /*
     * - start and length are in bytes, not in lights
     * - if sync is true, the inputSlicer will not load new data from its sourceInput until this
     * destinationInput is consumed. By default sync is true for all slices, which means that new input
     * data will only be requested when all slices are done. If you set one sync to false, that slice
     * is ignored. This can be useful if you have an output at a significantly lower rate, or lower priority
     * (like a monitorOutput)
     * if all slices are set to sync=false, the framerate is syncs to that of the input, guaranteeing that
     * each destination will always get the latest version of the data.
     * do not set sync=false for all slices when you are generating patterns, because this will
     * continuously generate new frames
     */
    typedef struct
    {
        int start;
        int length;
        bool sync;
    } Slice;

    Slicer(const std::vector<Slice> slices)
    {
        this->slices = slices;

        for (Slice slice : slices)
        {
            bufferSize = std::max(bufferSize, slice.start + slice.length);
        }

        buffer = (uint8_t *)malloc(bufferSize);
        if (!buffer)
        {
            Log::error("SLICER", "Unable to allocate buffer for slicer.");
        }

        for (Slice slice : slices)
        {
            auto bi = new BufferInput(this->buffer + slice.start, slice.length);
            this->destinationInputs.push_back(bi); 
        }
    }

    bool ready() override
    {
        int index = 0;
        for (auto di : destinationInputs)
        {
            // if this output not marked as sync, ignore it and continue to the next
            if (!slices[index++].sync)
                continue;

            // the output is marked as sync. If the bufferOutput still has a frame ready to be
            // sent out (or is in the process of sending it out), we are not ready to load new data yet.
            if (di->getFrameReady())
                return false;
        }
        return true;
    }

    void setLength(int length) override
    {
        // do nothing
    }

    void setData(uint8_t *data, int size) override
    {
        memcpy(buffer, data, std::min(size, bufferSize));
    }

    void show() override
    {
        for (auto di : destinationInputs)
        {
            di->setFrameReady();
        }
    }

    ISender *getSlice(int index)
    {
        return destinationInputs[index];
    }

    int size()
    {
        return destinationInputs.size();
    }

protected:
    std::vector<BufferInput *> destinationInputs;
    std::vector<Slice> slices;
    uint8_t *buffer;
    int bufferSize = 0;
};
