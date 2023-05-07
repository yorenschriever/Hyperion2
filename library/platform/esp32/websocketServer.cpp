#include "websocketServer.hpp"

std::unique_ptr<WebsocketServer> WebsocketServer::createInstance(unsigned short port)
{
    return std::unique_ptr<WebsocketServer>(nullptr);
}