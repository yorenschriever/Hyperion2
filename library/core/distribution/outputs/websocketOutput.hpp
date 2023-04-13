#pragma once

#include "output.hpp"
#include "core/distribution/utils/hostnameCache.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/websocketServer.hpp"
#include "platform/includes/thread.hpp"
#include "platform/includes/utils.hpp"
#include <algorithm>

// WebsocketOutput sends data to websocket clients.
class WebsocketOutput : public Output
{
public:
    WebsocketOutput(int port, unsigned int fps)
    {
        this->port = port;
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
        server->sendAll(buffer,length);
    }

    void postProcess() override
    {
    }

    void begin() override
    {
        lastFrame = Utils::millis();
        if (server.get() == nullptr)
            server = WebsocketServer::createInstance(this->port);
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
    int port;
    int frameInterval;
    unsigned long lastFrame = 0;

    int length;
    uint8_t *buffer;

    std::unique_ptr<WebsocketServer> server;

    const char* TAG = "WEBSOCKET_OUTPUT";
};
