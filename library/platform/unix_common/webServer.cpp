#include "webServer.hpp"
#include "webServer-unix.hpp"

WebServer* WebServer::createInstance(int port)
{
    return new WebServerUnix(WEBSERVER_ROOT, port);
}

std::unique_ptr<WebsocketServer> WebsocketServer::createInstance(WebServer *server, const char * path)
{
    return std::unique_ptr<WebsocketServerUnix>(new WebsocketServerUnix(server,path));
}