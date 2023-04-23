#pragma once

#include <algorithm>
#include <cstring>
#include <vector>
#include "input.hpp"
#include "bufferInput.hpp"
#include "utils.hpp"

// InputSplitter splits the input into separate inputs that can be used in different pipelines.
// I wrote an InputSplitter instead of an OutputSplitter, so you have multiple pipes.
// This has advantages:
// - Each pipe can do its own colour conversion, so you can use multiple types of lights
// - Each pipe runs at its own fps, so the fastest output is not limited by the throughput of
//   the slowest Output.
class InputSplitter : public InputSlicer {

public: 
    /*
    * - sourceInput is the input to distribute over the destination inputs
    * - length is in bytes, not in lights
    * - if sync is on, a new frame will not be loaded until all outputs have finished sending their data.
    * This is useful when the sourceInput is an input that generates data on demand instead of listening
    * to incoming data. Without sync it would generate a new frame when any of the destination outputs
    * requests new data.
    * If sync is false you basically sync to the input.
    */
    InputSplitter(Input *sourceInput, std::vector<int> lengths, bool sync = false) : InputSlicer( sourceInput, {}, sync)
    {
        int start = 0;
        for (int length : lengths)
        {
            auto bi = new BufferInput(this->buffer + start, length);
            bi->setCallbacks(LoadData, Begin, this);
            this->destinationInputs.push_back(bi);
            start += length;
        }
    }
}