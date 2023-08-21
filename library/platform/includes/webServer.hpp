#pragma once
#include <inttypes.h>
#include <string>
#include "webServerResponseBuilder.hpp"

class WebServerResponseBuilder;

class WebServer
{
public:
    static WebServer *createInstance(int port = 80);

    virtual ~WebServer() = default;

    virtual void addPath(std::string path, WebServerResponseBuilder *builder) = 0;

protected: 
    WebServer(){}
};
