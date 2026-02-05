#pragma once
#include "../interfaces.hpp"
#include "log.hpp"
#include <vector>

class Combine;

class ICombine
{
public:
    virtual void processCombined(Buffer inputBuffer, int offset) = 0;
    virtual void resizeBuffer() = 0;
};

class CombinedInputPart final : public ISink
{
    friend Combine;

public:
    CombinedInputPart(ICombine *parent, int offset, bool sync)
    {
        this->parent = parent;
        this->offset = offset;
        this->sync = sync;
    }

    void initialize() override { }

    void process(const Buffer inputBuffer) override
    {
        if (inputBuffer.size() != length)
        {
            length = inputBuffer.size();
            parent->resizeBuffer();
        }
        parent->processCombined(inputBuffer, offset);
        dirty = true;

        fpsCounter.increaseUsedFrameCount();
    }

    bool ready() override
    {
        return !dirty;
    }

    FPSCounter *getFpsCounter() override
    {
        return &fpsCounter;
    }
private:
    ICombine *parent;
    int offset = 0;
    int length = 0;
    bool dirty = false;
    bool sync;
    FPSCounter fpsCounter;
};

// CombinedInput reads the input from multiple sources and glues them together
class Combine final : public ISource, ICombine
{
    friend CombinedInputPart;

public:
    CombinedInputPart *atOffset(int offset, bool sync = true)
    {
        auto part = new CombinedInputPart(this, offset, sync);
        parts.push_back(part);
        return part;
    }

    CombinedInputPart *atDmxChannel(int offset, bool sync = true)
    {
        return atOffset(offset-1, sync);
    }

    FPSCounter *getFpsCounter() override    
    {
        return &fpsCounter;
    }

private:
    std::vector<CombinedInputPart *> parts;
    Buffer buffer = Buffer(0);
    FPSCounter fpsCounter;

    void resizeBuffer() override
    {
        int maxLength = 0;
        for (auto part : parts)
            maxLength = std::max(maxLength, part->offset + part->length);

        buffer = Buffer(maxLength);
    }

    void processCombined(Buffer inputBuffer, int offset) override
    {
        memcpy((uint8_t*)buffer.data() + offset, inputBuffer.data(), inputBuffer.size());

        for (auto part : parts)
            part->dirty = false;

        //todo missedframecount?
    }

    void initialize() override
    {
    }

    bool ready() override
    {
        for (auto part : parts)
            if (part->dirty)
                return true;
        return false;
    }

    Buffer process() override
    {
        fpsCounter.increaseUsedFrameCount();
        return buffer;
    }
};