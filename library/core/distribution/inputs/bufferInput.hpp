#pragma once
#include "input.hpp"
#include <algorithm>

// Use bufferInput when you manually maintain a buffer with pixelData,
// and want to feed it into a pipe
class BufferInput final : public Input
{
    using LoadDataCallback = void (*)(void *);
    using BeginCallback = void (*)(void *);

public:
    BufferInput(uint8_t *buffer, unsigned int length)
    {
        setBuffer(buffer, length);
    }

    void setBuffer(uint8_t *buffer, unsigned int length)
    {
        this->buffer = buffer;
        this->length = length;
    }

    // call this when you updated your buffer with new data
    void setFrameReady()
    {
        frameReady = true;
        missedframecount++;
    }

    bool getFrameReady()
    {
        return frameReady;
    }

    virtual void begin()
    {
        if (beginCallback)
            beginCallback(callbackParam);
    }

    virtual int loadData(uint8_t *dataPtr, unsigned int buffersize)
    {
        if (loadDataCallback)
            loadDataCallback(callbackParam);

        if (!frameReady)
            return 0;

        frameReady = false;

        memcpy(dataPtr, buffer, std::min(length, buffersize));

        usedframecount++;
        missedframecount--;

        return length;
    }

    void setCallbacks(LoadDataCallback loadDataCallback, BeginCallback beginCallback, void *callbackParam)
    {
        this->loadDataCallback = loadDataCallback;
        this->beginCallback = beginCallback;
        this->callbackParam = callbackParam;
    }

private:
    uint8_t *buffer;
    unsigned int length = 0;
    bool frameReady = false;
    LoadDataCallback loadDataCallback = NULL;
    BeginCallback beginCallback = NULL;
    void *callbackParam = NULL;
};