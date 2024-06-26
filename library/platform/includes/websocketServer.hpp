#pragma once
#include <deque>
#include <inttypes.h>
#include <string>
#include <stdarg.h>
#include <memory>
#include "webServer.hpp"

typedef void RemoteWebsocketClient;

class WebsocketServer
{
public:
    static std::unique_ptr<WebsocketServer> createInstance(WebServer *server, const char * path);

    using WebsocketMessageHandler = void (*)(RemoteWebsocketClient *client, WebsocketServer *server, std::string, void* userData);
    using WebsocketBinaryHandler = void (*)(RemoteWebsocketClient *client, WebsocketServer *server, uint8_t *data, int len, void* userData);
    using WebsocketConnectionHandler = void (*)(RemoteWebsocketClient *client, WebsocketServer *server, void* userData);

    virtual ~WebsocketServer() = default;

    virtual void send(RemoteWebsocketClient *client, std::string msg) = 0;
    virtual void sendOther(RemoteWebsocketClient *exclude, std::string msg) = 0;
    virtual void sendAll(std::string msg) = 0;

    virtual void send(RemoteWebsocketClient *client, const char* msg, ...) = 0;
    virtual void sendOther(RemoteWebsocketClient *exclude, const char* msg, ...) = 0;
    virtual void sendAll(const char* msg, ...) = 0;

    virtual void send(RemoteWebsocketClient *client, uint8_t *bytes, int size) = 0;
    virtual void sendOther(RemoteWebsocketClient *exclude, uint8_t *bytes, int size) = 0;
    virtual void sendAll(uint8_t *bytes, int size) = 0;

    virtual int connectionCount() = 0;
    virtual void onMessage(WebsocketMessageHandler handler, void* userData=0)
    {
        this->handler = handler;
        this->userData = userData;
    }
    virtual void onBinary(WebsocketBinaryHandler binaryHandler, void* binaryUserData=0)
    {
        this->binaryHandler = binaryHandler;
        this->binaryUserData = binaryUserData;
    }
    virtual void onConnect(WebsocketConnectionHandler connectionHandler, void* connectionUserData=0)
    {
        this->connectionHandler = connectionHandler;
        this->connectionUserData = connectionUserData;
    }
    virtual void onDisconnect(WebsocketConnectionHandler disconnectionHandler, void* disconnectionUserData=0)
    {
        this->disconnectionHandler = disconnectionHandler;
        this->disconnectionUserData = disconnectionUserData;
    }

protected:
    WebsocketMessageHandler handler = NULL;
    WebsocketBinaryHandler binaryHandler = NULL;
    WebsocketConnectionHandler connectionHandler = NULL;
    WebsocketConnectionHandler disconnectionHandler = NULL;
    void* userData;
    void* binaryUserData;
    void* connectionUserData;
    void* disconnectionUserData;
    WebsocketServer(){}
};
