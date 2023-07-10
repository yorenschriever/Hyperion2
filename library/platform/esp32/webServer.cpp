#include "webServer.hpp"
#include "webServer-esp.hpp"

WebServer* WebServer::createInstance(int port)
{
    return new WebServerEsp(port);
}