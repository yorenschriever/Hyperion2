#pragma once
#include "log.hpp"
#include "input.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/thread.hpp"
#include "platform/includes/utils.hpp"
#include "platform/includes/websocketServer.hpp"
#include "platform/includes/webServer.hpp"
#include "utils.hpp"
#include <algorithm>

// WebsocketInput opens a websocket, and listens for incoming frames.
class WebsocketInput : public Input
{

public:
    WebsocketInput(WebServer **server, std::string path, bool ack = false)
    {
        this->server = server;
        this->path = path;
        this->ack = ack;
    }

    virtual void begin() override
    {
        if (path.rfind("/ws/", 0) != 0){
            Log::error(TAG, "Websocket path should start with /ws/. (%s)", path.c_str());
            return;
        }
        socket = WebsocketServer::createInstance(*server,path.c_str());
        socket->onBinary(handler, (void *)this);
    }

    static void handler(RemoteWebsocketClient *client, WebsocketServer *server, uint8_t* data, int len, void *userData)
    {
        auto *instance = (WebsocketInput *)userData;
        if (len > instance->bufferSize)
            instance->buffer = (uint8_t*)realloc(instance->buffer,len);
        
        memcpy(instance->buffer, data, len);
        instance->dataSize = len;
        instance->frameReady = true;

        instance->missedframecount++;
    }

    virtual int loadData(uint8_t *dataPtr, unsigned int buffersize) override
    {
        if (!frameReady)
            return 0;

        if (ack)
            socket->sendAll("OK");

        frameReady = false;

        memcpy(dataPtr, buffer, std::min(dataSize, buffersize));

        usedframecount++;
        missedframecount--;

        return dataSize;
    }

private:
    const char* TAG = "WebsocketInput";
    uint8_t* buffer = nullptr;
    int bufferSize=0;
    unsigned int dataSize=0;
    bool frameReady = false;
    bool ack= false;
    std::unique_ptr<WebsocketServer> socket=0;
    WebServer **server;
    std::string path;
};