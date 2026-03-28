#pragma once

#include "baseOutput.hpp"
#include "utils.hpp"
#include <algorithm>

class BufferOutput final : public BaseOutput
{
public:
    BufferOutput(uint8_t *buffer, size_t length)
    {
        this->buffer = buffer;
        this->length = length;
    }

    bool ready() override
    {
        return true;
    }

    void process(const Buffer &inputBuffer) override
    {
        memcpy(buffer, inputBuffer.data(), std::min(inputBuffer.size(), length));
    }

    void initialize() override
    {
    }

private:
    uint8_t *buffer;
    size_t length;
};