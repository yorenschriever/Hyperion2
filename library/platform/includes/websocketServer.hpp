#pragma once
#include <deque>
#include <inttypes.h>
#include <string>

typedef void RemoteWebsocketClient;

class WebsocketServer
{
public:
    static std::unique_ptr<WebsocketServer> createInstance(unsigned short port);

    using WebsocketMessageHandler = void (*)(RemoteWebsocketClient *client, WebsocketServer *server, std::string);

    virtual ~WebsocketServer() = default;

    virtual void send(RemoteWebsocketClient *client, std::string msg) = 0;
    virtual void sendOther(RemoteWebsocketClient *exclude, std::string msg) = 0;
    virtual void sendAll(std::string msg) = 0;

    virtual void send(RemoteWebsocketClient *client, uint8_t *bytes, int size) = 0;
    virtual void sendOther(RemoteWebsocketClient *exclude, uint8_t *bytes, int size) = 0;
    virtual void sendAll(uint8_t *bytes, int size) = 0;

    virtual int connectionCount() = 0;
    virtual void onMessage(WebsocketMessageHandler handler)
    {
        this->handler = handler;
    }

protected:
    WebsocketMessageHandler handler = NULL;
};
