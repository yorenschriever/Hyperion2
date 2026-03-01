
#pragma once

#include "../interfaces.hpp"
#include "platform/includes/utils.hpp"
#include "platform/includes/webServer.hpp"
#include "platform/includes/websocketServer.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/thread.hpp"
#include <string>
#include <algorithm>

class Analytics final:  public IConverter
{

    class AnalyticsServer final
    {
        public:
        static AnalyticsServer *getInstance(WebServer **webServer)
        {
            static AnalyticsServer instance = AnalyticsServer(webServer);
            return &instance;
        }

        void addAnalyticsSource(Analytics *source)
        {
            sources.push_back(source);
        }

        private:
        AnalyticsServer(WebServer **webServer) {
            if ((*webServer) == nullptr)
            {
                Log::error(TAG, "Cannot construct Analytics without a server");
                Utils::exit();
            }
            server = WebsocketServer::createInstance(*webServer, "/ws/analytics");
            Thread::create(analyticsTask, "AnalyticsTask", Thread::Purpose::control, 3000, this, 4);
        }

        static void analyticsTask(void *param)
        {
            AnalyticsServer *instance = (AnalyticsServer *)param;
            unsigned long lastSent = Utils::millis();
            unsigned long now = Utils::millis();
            while (true)
            {
                Thread::sleep(1000); 
                lastSent = now;
                now = Utils::millis();
                int elapsed = now - lastSent;

                for (Analytics *source : instance->sources)
                {
                    int fps = source->frames * 1000 / elapsed;
                    source->frames = 0; 

                    auto ip = source->host ? HostnameCache::lookup(source->host) : nullptr;
                    std::string ipStr = ip != nullptr ? ip->toString() : "";

                    char buffer[100];
                    int sz = snprintf(buffer, sizeof(buffer), "{\"name\":\"%s\",\"fps\":%d,\"numLights\":%d, \"ip\":\"%s\"}\n", source->name.c_str(), fps, source->lastFrameSize/(int)source->colorSize, ipStr.c_str());
                    
                    instance->server->sendAll((char *)buffer, sz);
                }
            }
        }

        std::vector<Analytics *> sources;
        std::unique_ptr<WebsocketServer> server;
        const char *TAG = "ANALYTICS_SERVER";
    }; 


    friend class AnalyticsServer;
    AnalyticsServer *server;
    size_t colorSize;
    int lastFrameSize = 0;
    int frames = 0;
    std::string name;
    char *host;
    WebServer **webServer;

public: 

    Analytics(WebServer **webServer, std::string name, size_t colorSize = sizeof(RGB), char* host = nullptr)
    {
        this->webServer = webServer;
        this->name = name;
        this->colorSize = colorSize;
        this->host = host;
    }

    void initialize() override {
        server = AnalyticsServer::getInstance(webServer);
        server->addAnalyticsSource(this);
    }

    bool ready() override { return true; }

    const Buffer process(const Buffer& inputBuffer) override
    {
        lastFrameSize = inputBuffer.size();
        frames++;
        return inputBuffer;
    }
    
    
};

