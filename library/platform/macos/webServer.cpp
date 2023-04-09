#include "webServer.hpp"
#include "webServer-macos.hpp"

WebServer* WebServer::createInstance(std::string doc_root, int port)
{
    return new WebServerMacOs(doc_root, port);
}