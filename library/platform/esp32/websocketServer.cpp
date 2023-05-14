#include "websocketServer.hpp"
#include "websocketServer-esp.hpp"

std::unique_ptr<WebsocketServer> WebsocketServer::createInstance(unsigned short port)
{
    return std::unique_ptr<WebsocketServerEsp>(new WebsocketServerEsp(port));
}