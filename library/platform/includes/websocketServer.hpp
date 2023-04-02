#pragma once
#include <deque>
#include <inttypes.h>
#include <string>

class RemoteWebsocketClient;

class WebsocketServer
{
public:
    static WebsocketServer *createInstance();

    using WebsocketMessageHandler = void (*)(RemoteWebsocketClient *client, WebsocketServer *server, std::string);

    virtual void send(RemoteWebsocketClient *client, std::string msg) = 0;
    virtual void sendOther(RemoteWebsocketClient *exclude, std::string msg) = 0;
    virtual void sendAll(std::string msg) = 0;

    virtual int connectionCount() = 0;
    virtual void onMessage(WebsocketMessageHandler handler)
    {
        this->handler = handler;
    }

    virtual int readMessage(uint8_t *buffer) = 0;

    void setRxBufferSize(int size)
    {
        maxBufferSize = size;
    }

protected:
    WebsocketMessageHandler handler = NULL;
    std::deque<std::string *> bufferedMessages = std::deque<std::string *>();
    int maxBufferSize = 5;
};
