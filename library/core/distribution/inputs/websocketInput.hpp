#pragma once
#include "baseInput.hpp"
#include "log.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/thread.hpp"
#include "platform/includes/utils.hpp"
#include "platform/includes/webServer.hpp"
#include "platform/includes/websocketServer.hpp"
#include "utils.hpp"
#include <algorithm>

const int MTU = 1400;

// WebsocketInput opens a websocket, and listens for incoming frames.
class WebsocketInput final: public BaseInput
{

public:
    WebsocketInput(WebServer **server, std::string path, bool ack = false)
    {
        this->server = server;
        this->path = path;
        this->ack = ack;
    }

    void initialize() override
    {
        if (path.rfind("/ws/", 0) != 0){
            Log::error(TAG, "Websocket path should start with /ws/. (%s)", path.c_str());
            return;
        }
        socket = WebsocketServer::createInstance(*server, path.c_str());
        socket->onBinary(handler, (void *)this);
    }

    static void handler(RemoteWebsocketClient *client, WebsocketServer *server, uint8_t *data, int len, void *userData)
    {
        auto *instance = (WebsocketInput *)userData;
        if (len > instance->bufferSize)
            instance->buffer = (uint8_t *)realloc(instance->buffer, len);

        memcpy(instance->buffer, data, len);
        instance->dataSize = len;
        instance->frameReady = true;

        instance->fpsCounter.increaseMissedFrameCount();
    }

    Buffer *getData() override
    {
        if (!frameReady)
            return nullptr;

        if (ack)
            socket->sendAll("OK");

        frameReady = false;

        const unsigned int bufferSize = 2 * MTU;
        auto patternBuffer = BufferPool::getBuffer(bufferSize);
        if (!patternBuffer)
        {
            Log::error("PATTERN_INPUT", "Unable to allocate memory for PatternInput, free heap = %d\n", Utils::get_free_heap());
            Utils::exit();
        }
        auto dataPtr = patternBuffer->getData();

        memcpy(dataPtr, buffer, std::min(dataSize, bufferSize));

        fpsCounter.getUsedFrameCount();
        fpsCounter.increaseMissedFrameCount(-1);

        return patternBuffer;
    }

private:
    const char *TAG = "WebsocketInput";
    uint8_t *buffer = nullptr;
    int bufferSize = 0;
    unsigned int dataSize = 0;
    bool frameReady = false;
    bool ack = false;
    std::unique_ptr<WebsocketServer> socket = 0;
    WebServer **server;
    std::string path;
};