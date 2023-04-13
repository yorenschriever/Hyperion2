#pragma once

#include <algorithm>
#include <cstring>
#include <vector>
#include "input.hpp"
#include "bufferInput.hpp"
#include "utils.hpp"


// InputSlicer is a more advanced version of input splitter. 
// With this class you can freely assing start and end positions
// of each slice
class InputSlicer
{

public:
    /*
    * - sourceInput is the input to distribute over the destination inputs
    * - length is in bytes, not in lights
    * - if sync is on, a new frame will not be loaded until all outputs have finished sending their data.
    * This is useful when the sourceInput is an input that generates data on demand instead of listening
    * to incoming data. Without sync it would generate a new frame when any of the destination outputs
    * requests new data.
    */
    typedef struct {
        int start;
        int length;
    } Slice;

    InputSlicer(Input *sourceInput, std::vector<Slice> slices, bool sync = false)
    {
        this->sourceInput = sourceInput;
        this->sync=sync;
        for (Slice slice : slices)
        {
            auto bi = new BufferInput(this->buffer + slice.start, slice.length);
            bi->setCallbacks(LoadData, Begin, this);
            this->destinationInputs.push_back(bi);
        }
    }

    static void Begin(void *argument)
    {
        auto instance = (InputSlicer *)argument;
        if (instance->begun)
          return;

        instance->begun = true;
        instance->sourceInput->begin();
    }

    static void LoadData(void *argument)
    {       
        auto instance = (InputSlicer *)argument;

        //check all outputs, and ask if there is still a frame pending
        if (instance->sync){
            for (auto di : instance->destinationInputs)
            {
                if (di->getFrameReady()){
                    return;
                }
            }
        }

        auto len = instance->sourceInput->loadData(instance->buffer, MTU);
        if (len > 0)
        {   
            for (auto di : instance->destinationInputs)
            {
                di->setFrameReady();
            }
        }
    }

    Input *getInput(int index)
    {
        return destinationInputs[index];
    }

    int size()
    {
        return destinationInputs.size();
    }

private:
    Input *sourceInput = NULL;
    std::vector<BufferInput *> destinationInputs;
    static const int MTU = 1500 * 4 * 6;
    uint8_t buffer[MTU];
    bool sync;
    bool begun = false;
};
