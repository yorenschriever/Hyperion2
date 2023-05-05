#pragma once
#include <inttypes.h>
#include <string>

class WebServer;

class WebServerResponseBuilder
{
public:
    using Writer = void (*)(const char *buffer, int size, void* userData);
    virtual void build(Writer write, void* userData) = 0;
};
