#include "websocketServer.hpp"
#include "websocketServer-esp.hpp"
#include "webServer-esp.hpp"

std::unique_ptr<WebsocketServer> WebsocketServer::createInstance(WebServer *server, const char * path)
{
    return std::unique_ptr<WebsocketServerEsp>(new WebsocketServerEsp((WebServerEsp *)server, path));
}