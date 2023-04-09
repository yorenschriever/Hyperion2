#pragma once
#include <inttypes.h>
#include <string>

class WebServerResponseBuilder;

class WebServer
{
public:
    static WebServer *createInstance(std::string doc_root = WEBSERVER_ROOT, int port = 443);

    virtual void addPath(std::string path, WebServerResponseBuilder *builder) = 0;
};
