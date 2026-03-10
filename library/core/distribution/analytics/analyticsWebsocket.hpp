#pragma once

#include "../interfaces.hpp"
#include "analyticsHub.hpp"
#include "core/distribution/utils/hostnameCache.hpp"
#include "platform/includes/socket.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/utils.hpp"
#include "platform/includes/webServer.hpp"
#include "platform/includes/websocketServer.hpp"
#include <algorithm>
#include <string>

class AnalyticsWebsocket final : public AnalyticsHub::Destination
{

public:
    AnalyticsWebsocket(WebServer *server)
    {
        if (server == nullptr)
        {
            Log::error(TAG, "Cannot construct AnalyticsWebsocket without a server");
            Utils::exit();
        }
        this->websocketServer = WebsocketServer::createInstance(server, "/ws/analytics");
        this->udpSocket = new Socket(AnalyticsHub::analyticsUdpPort, AF_INET, 0, true);

        AnalyticsHub::getInstance()->addAnalyticsDestination(this);
    }

    void sendAnalytics(const std::vector<AnalyticsHub::Analytics> &analyticsData)
    {
        char buffer[AnalyticsHub::analyticsLineBufferSize];

        // send local analytics to websocket clients
        for (auto &analytics : analyticsData)
        {
            int sz = AnalyticsHub::formatAnalyticsLine(buffer, sizeof(buffer), analytics, "local");
            websocketServer->sendAll(buffer, sz);
        }

        //repeat any received UDP analytics to websocket clients
        int sz;
        while ((sz = udpSocket->receive(buffer, sizeof(buffer))) > 0)
            websocketServer->sendAll(buffer, sz);
    }

private:
    Socket *udpSocket;
    std::unique_ptr<WebsocketServer> websocketServer;
    const char *TAG = "ANALYTICS_WEBSOCKET";
};