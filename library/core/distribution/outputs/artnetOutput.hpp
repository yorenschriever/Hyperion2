#pragma once

#include "platform/includes/artNet.hpp"
#include "baseOutput.hpp"
#include "utils.hpp"

class ArtNetOutput : public BaseOutput
{
public:
    ArtNetOutput(const char* hostname, uint8_t net, uint8_t subnet, uint8_t universe, unsigned int fps=60)
    {
        this->hostname = hostname;
        this->net = net;
        this->subnet = subnet;
        this->universe = universe;
        this->frameInterval = 1000 / fps;
    }

    // size is in bytes
    void setData(uint8_t *data, int size) override
    {
        int copy_length = std::min(size, length);
        if (copy_length > 0)
            memcpy(this->buffer, data, copy_length);
    }

    bool ready() override
    {
        if (!artNet) return false;
        return (Utils::millis() - lastFrame >= frameInterval);
    }

    void show() override
    {
        if (!ready())
            return;

        lastFrame = Utils::millis();
        artNet->send(hostname, net, subnet, universe, buffer, length);
    }

    void begin() override
    {
        lastFrame = Utils::millis();
        this->artNet = ArtNet::getInstance();
    }

    void clear() override
    {
        memset(buffer, 0, length);
    }

    // length is in bytes
    void setLength(int len) override
    {
        if (len == this->length)
            return;
        if (len > 512)
            return;
        
        this->length = len;
    }

private:
    const char *hostname;
    uint8_t net;
    uint8_t subnet;
    uint8_t universe;
    ArtNet *artNet = nullptr;

    uint8_t buffer[512];
    int length = 512;

    int frameInterval;
    unsigned long lastFrame = 0;
};