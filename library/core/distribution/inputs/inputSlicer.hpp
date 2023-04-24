#pragma once

#include <algorithm>
#include <cstring>
#include <vector>
#include "input.hpp"
#include "bufferInput.hpp"
#include "utils.hpp"


// InputSlicer is a more advanced version of input splitter. 
// With this class you can freely assign start and end positions
// of each slice. This means you can also skip parts of the buffer,
// or use parts twice.
class InputSlicer
{

public:
    /*
    * - sourceInput is the input to distribute over the destination inputs
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
    typedef struct  {
        int start;
        int length;
        bool sync;
    } Slice;

    InputSlicer(Input *sourceInput, const std::vector<Slice> slices)
    {
        this->sourceInput = sourceInput;
        this->slices = slices;

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


        int index=0;
        for (auto di : instance->destinationInputs)
        {
            //if this output not markes as sync, ignore it and continue to the next
            if (!instance->slices[index++].sync)
                continue;

            //the output is marked as sync. If the bufferOutput still has a frame ready to be
            //send out (or is in the process of sending it out), we are not ready to load new data yet.
            if (di->getFrameReady()){
                return;
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

protected:
    Input *sourceInput = NULL;
    std::vector<BufferInput *> destinationInputs;
    std::vector<Slice> slices;
    static const int MTU = 1500 * 4 * 6;
    uint8_t buffer[MTU];
    bool begun = false;
};
