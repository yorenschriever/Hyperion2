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
        const char *host;
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

private:
    AnalyticsHub()
    {
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