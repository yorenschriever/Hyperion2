#include "websocketServer.hpp"
#include "websocketServer-macos.hpp"

WebsocketServer* WebsocketServer::createInstance()
{
    return new WebsocketServerMacOs();
}