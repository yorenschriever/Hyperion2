#pragma once

#include "core/generation/pixelMap.hpp"
#include "output.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/pixelMonitor3d.hpp"
#include "platform/includes/webServer.hpp"
#include "platform/includes/webServerResponseBuilder.hpp"
#include "websocketOutput.hpp"
#include <stdarg.h>
#include "log.hpp"

class PixelMapJson : public WebServerResponseBuilder
{
    
    void build(WebServerResponseBuilder::Writer writer, void *userData) override
    {
        //Log::info("","Building mapping json. ");
        write(userData, writer, "[\n");
        int outputIndex = 0;
        for (auto output : outputs)
        {
            write(userData, writer, "  {\n");
            write(userData, writer, "    \"port\": %d,\n", output.port);
            write(userData, writer, "    \"positions\": [\n");
            int index = 0;
            for (auto pixel : output.map)
                write(userData, writer, "      {\"x\" : %f, \"y\" : %f, \"z\" : %f}%s \n", pixel.x, pixel.y, pixel.z, index++ == output.map.size() - 1 ? "" : ",");
            write(userData, writer, "    ]\n");
            write(userData, writer, "  }%s\n", outputIndex++ == outputs.size() - 1 ? "" : ",");
        }
        write(userData, writer, "]\n");
    }

    std::vector<PixelMap3d> maps;

public:
    unsigned int addOutput(PixelMap3d map)
    {
        outputs.push_back({.map = map, .port = ++port});
        return port;
    }

private:
    struct PixelMonitorOutput3d
    {
        PixelMap3d map;
        unsigned int port;
    };

    std::vector<PixelMonitorOutput3d> outputs;
    bool begun;
    unsigned int port = 9700;
    char buffer[100];

    void write(void *userData, WebServerResponseBuilder::Writer writer, const char *message, ...)
    {
        va_list args;
        va_start(args, message);
        int sz = vsnprintf(buffer, sizeof(buffer), message, args);
        writer(buffer, sz, userData);
        va_end(args);
    }
};


// MonitorOutput displays the led data on your browser
class MonitorOutput3dws : public WebsocketOutput
{
public:
    MonitorOutput3dws(PixelMap3d map, WebServer *server, unsigned int fps = 60) : WebsocketOutput(0, fps)
    {
        this->map = map;
        this->port = pixelMapJson.addOutput(map);
        this->server = server;
    }

    void begin() override
    {
        WebsocketOutput::begin();
        server->addPath("/monitor/mapping.json",&pixelMapJson);
        //todo, handle case when different servers are passed for different instances
    }

private:
    PixelMap3d map;
    PixelMonitor3d monitor;
    WebServer *server;

    static PixelMapJson pixelMapJson;
};

PixelMapJson MonitorOutput3dws::pixelMapJson;