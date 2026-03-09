#pragma once

#include "../interfaces.hpp"
#include "analyticsHub.hpp"
#include "core/distribution/utils/hostnameCache.hpp"
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
        this->server = WebsocketServer::createInstance(server, "/ws/analytics");

        AnalyticsHub::getInstance()->addAnalyticsDestination(this);
    }

    void sendAnalytics(const std::vector<AnalyticsHub::Analytics> &analyticsData)
    {
        for (auto &source : analyticsData)
        {
            auto ip = source.host ? HostnameCache::lookup(source.host) : nullptr;
            std::string ipStr = ip != nullptr ? ip->toString() : "";

            char buffer[200];
            int sz = snprintf(buffer, sizeof(buffer), "{\"name\":\"%s\",\"fps\":%d,\"numLights\":%d, \"ip\":\"%s\"}\n", source.name.c_str(), source.fps, source.lastFrameSize / (int)source.colorSize, ipStr.c_str());
            server->sendAll(buffer, sz);
        }
    }

    std::unique_ptr<WebsocketServer> server;
    const char *TAG = "ANALYTICS_WEBSOCKET";
};