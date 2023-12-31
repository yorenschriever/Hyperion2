#include "webServer.hpp"
#include "webServer-esp.hpp"

WebServer* WebServer::createInstance(int port, bool secure)
{
    return new WebServerEsp(port, secure);
}