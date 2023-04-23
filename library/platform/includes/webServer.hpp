#pragma once
#include <inttypes.h>
#include <string>
#include "webServerResponseBuilder.hpp"

class WebServerResponseBuilder;

class WebServer
{
public:
    static WebServer *createInstance(std::string doc_root = WEBSERVER_ROOT, int port = 4430);

    virtual ~WebServer() = default;

    virtual void addPath(std::string path, WebServerResponseBuilder *builder) = 0;
};
