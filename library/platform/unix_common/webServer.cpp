#include "webServer.hpp"
#include "webServer-unix.hpp"

WebServer* WebServer::createInstance(int port, bool secure)
{
    //bool 'secure' is ignored, instead both, https and http calls wil be answered on this port
    return new WebServerUnix({WEBSERVER_PROJECT_ROOT, WEBSERVER_ROOT}, port);
}

std::unique_ptr<WebsocketServer> WebsocketServer::createInstance(WebServer *server, const char * path)
{
    return std::unique_ptr<WebsocketServerUnix>(new WebsocketServerUnix(server,path));
}