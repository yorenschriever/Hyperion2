#include "websocketServer.hpp"
#include "websocketServer-macos.hpp"

std::unique_ptr<WebsocketServer> WebsocketServer::createInstance(unsigned short port)
{
    return std::unique_ptr<WebsocketServerMacOs>(new WebsocketServerMacOs(port));
}