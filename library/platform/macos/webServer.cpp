#include "webServer.hpp"
#include "webServer-macos.hpp"

WebServer* WebServer::createInstance(int port)
{
    return new WebServerMacOs(WEBSERVER_ROOT, port);
}