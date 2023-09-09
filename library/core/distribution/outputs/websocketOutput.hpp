#pragma once

#include "core/distribution/utils/hostnameCache.hpp"
#include "output.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/thread.hpp"
#include "platform/includes/utils.hpp"
#include "platform/includes/webServer.hpp"
#include "platform/includes/websocketServer.hpp"
#include "utils.hpp"
#include <algorithm>

// WebsocketOutput sends data to websocket clients.
class WebsocketOutput : public Output
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

    // index and size are in bytes
    void setData(uint8_t *data, int size, int index) override
    {
        int copy_length = std::min(size, length - index);
        if (copy_length > 0)
            memcpy(this->buffer + index, data, copy_length);
    }

    bool ready() override
    {
        return (Utils::millis() - lastFrame >= frameInterval);
    }

    void show() override
    {
        lastFrame = Utils::millis();
        server->sendAll(buffer, length);
    }

    void postProcess() override
    {
    }

    void begin() override
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

    void clear() override
    {
        memset(this->buffer, 0, this->length);
    }

    // length is in bytes
    void setLength(int len) override
    {
        if (len != this->length)
        {
            // wait for the front buffer to be sent before we are going to change its size
            while (!ready())
                Thread::sleep(1);

            buffer = (uint8_t *)realloc(buffer, len);

            if (!buffer)
            {
                Log::error(TAG, "Unable to allocate memory for WebsocketOutput, free heap = %d\n", Utils::get_free_heap());
                Utils::exit();
            }

            this->length = len;
        }
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
