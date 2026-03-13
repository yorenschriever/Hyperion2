#pragma once

#include "../interfaces.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/thread.hpp"
#include "platform/includes/utils.hpp"
#include "platform/includes/webServer.hpp"
#include "platform/includes/websocketServer.hpp"
#include <algorithm>
#include <string>

const int ANALYTICS_INTERVAL = 1000; // in ms

class AnalyticsHub final
{
public:
    struct Analytics
    {
        const std::string name;
        size_t colorSize = 1;
        int lastFrameSize = 0;
        int fps = 0;
    };

    class Source
    {
    public:
        virtual AnalyticsHub::Analytics calculate(int elapsedTime) = 0;
    };

    class Destination
    {
    public:
        virtual void sendAnalytics(const std::vector<AnalyticsHub::Analytics> &analyticsData) = 0;
    };

    static AnalyticsHub *getInstance()
    {
        static AnalyticsHub *instance = new AnalyticsHub();
        return instance;
    }

    void addAnalyticsSource(Source *source)
    {
        sources.push_back(source);
    }

    void addAnalyticsDestination(Destination *destination)
    {
        destinations.push_back(destination);
    }

    std::vector<Source *> sources;
    std::vector<Destination *> destinations;
    static const int analyticsUdpPort = 9630;

    static const int analyticsLineBufferSize = 200;
    static int formatAnalyticsLine(char *buffer, int buffersize, const AnalyticsHub::Analytics &analytics, const char *source)
    {
        return snprintf(
            buffer,
            buffersize,
            "{\"name\":\"%s\",\"fps\":%d,\"numLights\":%d,\"uptime\":%d,\"source\":\"%s\"}",
            analytics.name.c_str(),
            analytics.fps,
            analytics.lastFrameSize / (int)analytics.colorSize,
            int(Utils::millis() - AnalyticsHub::startupTime)/1000,
            source);
    }

private:
    static unsigned long startupTime;

    AnalyticsHub()
    {
        startupTime = Utils::millis();
        Thread::create(analyticsTask, "AnalyticsTask", Thread::Purpose::control, 3000, this, 4);
    }

    static void analyticsTask(void *param)
    {
        AnalyticsHub *instance = (AnalyticsHub *)param;
        unsigned long lastSent = Utils::millis();
        unsigned long now = Utils::millis();
        while (true)
        {
            Thread::sleep(ANALYTICS_INTERVAL);
            lastSent = now;
            now = Utils::millis();
            int elapsed = now - lastSent;

            // Log::info("ANALYTICS_HUB", "Free heap: %d", Utils::get_free_heap());

            auto analyticsData = std::vector<Analytics>();
            analyticsData.reserve(instance->sources.size());
            for (auto source : instance->sources)
            {
                analyticsData.push_back(source->calculate(elapsed));
            }

            for (auto destination : instance->destinations)
            {
                destination->sendAnalytics(analyticsData);
            }
        }
    }
};

unsigned long AnalyticsHub::startupTime=0;