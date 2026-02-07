#pragma once

#include "core/distribution/utils/hostnameCache.hpp"
#include "baseOutput.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/thread.hpp"
#include "platform/includes/utils.hpp"
#include "platform/includes/webServer.hpp"
#include "platform/includes/websocketServer.hpp"
#include "utils.hpp"
#include <algorithm>

// WebsocketOutput sends data to websocket clients.
class WebsocketOutput: public BaseOutput
{
public:
    WebsocketOutput(WebServer **webserver, const char *path, unsigned int fps)
    {
        this->webserver = webserver;
        this->path = path;
        this->frameInterval = 1000 / fps;

        this->length = 12;
        buffer = (uint8_t *)malloc(12);
    }

    bool ready() override
    {
        return (Utils::millis() - lastFrame >= frameInterval);
    }

    void initialize() override 
    {
        lastFrame = Utils::millis();
        if (server.get() == nullptr)
        {
            if ((*webserver) == nullptr)
            {
                Log::error(TAG, "Cannot construct WebsocketOutput or MonitorOutput without a server");
                Utils::exit();
            }
            server = WebsocketServer::createInstance(*webserver, path);
            server->onConnect(connectionHandler, (void *)this);
        }
    }

    void process(const Buffer& inputBuffer) override
    {
        if (!ready())
            return;

        if (!inputBuffer.size())
            return;

        if (inputBuffer.size() != length){
            buffer = (uint8_t *)Utils::realloc_dma(buffer, inputBuffer.size());
            if (!buffer)
            {
                Log::error("WEBSOCKET_OUTPUT", "Unable to allocate memory for WebsocketOutput, free heap = %d", Utils::get_free_heap());
                Utils::exit();
            }
            length = inputBuffer.size();
            memset(buffer, 0, length);
        }

        lastFrame = Utils::millis();

        memcpy(this->buffer, inputBuffer.data(), length);

        server->sendAll(buffer, length);

        fpsCounter.increaseUsedFrameCount();
    }

protected:
    WebServer **webserver;
    const char *path;
    int frameInterval;
    unsigned long lastFrame = 0;

    int length;
    uint8_t *buffer;

    std::unique_ptr<WebsocketServer> server;

    const char *TAG = "WEBSOCKET_OUTPUT";

    // This method sends a string to the client that identifies the code that is running here.
    // This information is used when a websocket reconnects: if the id is the same,
    // we are likely dealing with a restored connection, but if the id is different
    // we could be dealing with a new iteration of a dev build. This resets and rebuilds the
    // internal state of the client so everything is guaranteed to be in sync with the new version.
    static void connectionHandler(RemoteWebsocketClient *client, WebsocketServer *server, void *userData)
    {
        server->sendAll(
            "{\
        \"type\":\"runtimeSessionId\",\
        \"value\":\"%d\"\
    }",
            Utils::getRuntimeSessionId());
    }
};
