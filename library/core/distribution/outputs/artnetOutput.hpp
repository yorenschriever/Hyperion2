#pragma once

#include "platform/includes/artNet.hpp"
#include "baseOutput.hpp"
#include "utils.hpp"
#include <algorithm>

class ArtNetOutput final: public BaseOutput
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

    bool ready() override
    {
        if (!artNet) return false;
        return (Utils::millis() - lastFrame >= frameInterval);
    }

    void process(Buffer inputBuffer) override {
        if (!ready())
            return;

        lastFrame = Utils::millis();

        int length = std::min((int)inputBuffer.size(), 512);
        artNet->send(hostname, net, subnet, universe, inputBuffer.data(), length);
    }

    void initialize() override
    {
        lastFrame = Utils::millis();
        this->artNet = ArtNet::getInstance();
    }

    void clear() override
    {
        auto blankBuffer = Buffer(512);
        blankBuffer.clear<Monochrome>();
        process(blankBuffer);
    }

private:
    const char *hostname;
    uint8_t net;
    uint8_t subnet;
    uint8_t universe;
    ArtNet *artNet = nullptr;

    int frameInterval;
    unsigned long lastFrame = 0;
};