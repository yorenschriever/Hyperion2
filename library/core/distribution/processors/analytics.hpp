
#pragma once

#include "../analytics/analyticsHub.hpp"
#include "../interfaces.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/thread.hpp"
#include "platform/includes/utils.hpp"
#include "platform/includes/webServer.hpp"
#include "platform/includes/websocketServer.hpp"
#include <algorithm>
#include <string>

class Analytics final : public IProcessor, public AnalyticsHub::Source
{
private:
    friend class AnalyticsHub;
    AnalyticsHub *hub;
    size_t colorSize;
    int lastFrameSize = 0;
    volatile int frames = 0;
    std::string name;

public:
    Analytics(std::string name, size_t colorSize = sizeof(RGB))
    {
        this->name = name;
        this->colorSize = colorSize;
    }

    void initialize() override
    {
        hub = AnalyticsHub::getInstance();
        hub->addAnalyticsSource(this);
    }

    bool ready() override { return true; }

    const Buffer process(const Buffer &inputBuffer) override
    {
        lastFrameSize = inputBuffer.size();
        frames++;
        return inputBuffer;
    }

    AnalyticsHub::Analytics calculate(int elapsedTime) override
    {
        int fps = frames * 1000 / elapsedTime;
        frames = 0;

        return AnalyticsHub::Analytics{name, colorSize, lastFrameSize, fps};
    }
};