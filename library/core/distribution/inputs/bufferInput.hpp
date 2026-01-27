#pragma once
#include "baseInput.hpp"
#include "log.hpp"
#include "utils.hpp"
#include <algorithm>

// Use bufferInput when you manually maintain a buffer with pixelData,
// and want to feed it into a pipe
class BufferInput final : public BaseInput
{
    using LoadDataCallback = void (*)(void *);
    using InitializeCallback = void (*)(void *);

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
        fpsCounter.increaseMissedFrameCount();
    }

    virtual bool ready() override
    {
        return frameReady;
    }

    void initialize() override {
        if (initializeCallback)
            initializeCallback(callbackParam);
    }

    Buffer process() override
    {

        auto patternBuffer = Buffer(length);

        if (loadDataCallback)
            loadDataCallback(callbackParam);

        frameReady = false;

        auto dataPtr = patternBuffer.data();
        memcpy(dataPtr, buffer, length);

        fpsCounter.increaseUsedFrameCount();
        fpsCounter.increaseUsedFrameCount(-1);

        return patternBuffer;
    }

    void setCallbacks(LoadDataCallback loadDataCallback, InitializeCallback initializeCallback, void *callbackParam)
    {
        this->loadDataCallback = loadDataCallback;
        this->initializeCallback = initializeCallback;
        this->callbackParam = callbackParam;
    }

private:
    uint8_t *buffer;
    unsigned int length = 0;
    bool frameReady = false;
    LoadDataCallback loadDataCallback = NULL;
    InitializeCallback initializeCallback = NULL;
    void *callbackParam = NULL;
};