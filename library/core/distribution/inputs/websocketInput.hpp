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

    virtual bool ready() override
    {
        return frameReady;
    }

    Buffer process() override
    {
        if (!frameReady)
            return Buffer(0);

        if (ack)
            socket->sendAll("OK");

        frameReady = false;

        auto patternBuffer = Buffer(dataSize);

        memcpy(patternBuffer.data(), buffer, dataSize);

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