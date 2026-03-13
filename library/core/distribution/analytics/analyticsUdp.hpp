#pragma once

#include "../interfaces.hpp"
#include "analyticsHub.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/utils.hpp"
#include "platform/includes/socket.hpp"
#include "platform/includes/ipAddress.hpp"
#include "platform/includes/network.hpp"
#include "platform/includes/ethernet.hpp"
#include <algorithm>
#include <string>

class AnalyticsUDP final : public AnalyticsHub::Destination
{
    static Socket *sock;
    IPAddress broadcast = IPAddress::broadcast();

public:
    AnalyticsUDP()
    {
        sock = new Socket(0, AF_INET, 0, true);
        AnalyticsHub::getInstance()->addAnalyticsDestination(this);
    }

    void sendAnalytics(const std::vector<AnalyticsHub::Analytics> &analyticsData)
    {
        for (auto &source : analyticsData)
        {
            char buffer[AnalyticsHub::analyticsLineBufferSize];
            // +1 for the trailing closing null character.
            int sz = 1+AnalyticsHub::formatAnalyticsLine(buffer, sizeof(buffer), source, "remote");
            sock->send(&broadcast, AnalyticsHub::analyticsUdpPort, (uint8_t *)buffer, sz);
        }
    }

    const char *TAG = "ANALYTICS_UDP";
};

Socket *AnalyticsUDP::sock;