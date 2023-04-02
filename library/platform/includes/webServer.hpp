#pragma once
#include <inttypes.h>
#include <string>

class WebServer
{
public:
    static WebServer *createInstance(int port);

    virtual void addPath(std::string path, uint8_t* content, int size) =0 ; 
};
