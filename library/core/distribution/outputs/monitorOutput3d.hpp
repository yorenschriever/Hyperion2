#pragma once

#include "core/generation/pixelMap.hpp"
#include "log.hpp"
#include "output.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/webServer.hpp"
#include "platform/includes/webServerResponseBuilder.hpp"
#include "websocketOutput.hpp"
#include <stdarg.h>

class PixelMap3dJson : public WebServerResponseBuilder
{

    void build(WebServerResponseBuilder::Writer writer, void *userData) override
    {
        // Log::info("","Building mapping json. ");
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

// MonitorOutput3d displays the led data on your browser
class MonitorOutput3d : public WebsocketOutput
{
public:
    MonitorOutput3d(PixelMap3d map, unsigned int fps = 60) : WebsocketOutput(0, fps)
    {
        this->port = pixelMap3dJson.addOutput(map);
    }

    void begin() override
    {
        // Log::info(TAG,"beginning MonitorOutput3d on port %d",port);
        WebsocketOutput::begin();
    }

    static void addPathToServer(WebServer *server)
    {
        server->addPath("/monitor/mapping.json", &pixelMap3dJson);
    }

private:
    // todo support multiple servers / pixel maps
    static PixelMap3dJson pixelMap3dJson;
};

PixelMap3dJson MonitorOutput3d::pixelMap3dJson;