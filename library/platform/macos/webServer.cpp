#include "webServer.hpp"
#include "webServer-macos.hpp"

WebServer* WebServer::createInstance(int port)
{
    return new WebServerMacOs(WEBSERVER_ROOT, port);
}

std::unique_ptr<WebsocketServer> WebsocketServer::createInstance(WebServer *server, const char * path)
{
    return std::unique_ptr<WebsocketServerMacOs>(new WebsocketServerMacOs(server,path));
}