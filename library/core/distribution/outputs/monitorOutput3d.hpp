#pragma once

#include "core/generation/pixelMap/pixelMap.hpp"
#include "log.hpp"
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
            write(userData, writer, "    \"path\": \"/ws/monitor3d%d\",\n", output.mapIndex);
            write(userData, writer, "    \"size\": %f,\n", output.size);
            write(userData, writer, "    \"positions\": [\n");
            int index = 0;
            for (auto pixel = output.map->begin(); pixel != output.map->end(); pixel++)
                write(userData, writer, "      {\"x\" : %f, \"y\" : %f, \"z\" : %f}%s \n", pixel->x, pixel->y, pixel->z, index++ == output.map->size() - 1 ? "" : ",");
            write(userData, writer, "    ]\n");
            write(userData, writer, "  }%s\n", outputIndex++ == outputs.size() - 1 ? "" : ",");
        }
        write(userData, writer, "]\n");
    }

    std::vector<PixelMap3d> maps;

public:
    unsigned int addOutput(PixelMap3d *map, float size)
    {
        outputs.push_back({.map = map, .mapIndex = ++mapIndex, .size=size});
        return mapIndex;
    }

private:
    struct PixelMonitorOutput3d
    {
        PixelMap3d *map;
        unsigned int mapIndex;
        float size;
    };

    std::vector<PixelMonitorOutput3d> outputs;
    bool begun;
    unsigned int mapIndex = 0;
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

// MonitorOutput3d displays the led data in your browser
class MonitorOutput3d final: public WebsocketOutput
{
public:
    MonitorOutput3d(WebServer **webServer, PixelMap3d *map, unsigned int fps = 60, float size = 0.01) : WebsocketOutput(webServer, pathBuf, fps)
    {
        this->webServer = webServer;
        snprintf(pathBuf, 20, "/ws/monitor3d%d", pixelMap3dJson.addOutput(map, size));
    }

    void initialize() override 
    {
        WebsocketOutput::initialize();
        (*webServer)->addPath("/monitor/mapping.json", &pixelMap3dJson);
    }

private:
    WebServer **webServer;
    char pathBuf[20];
    // todo support multiple pixel maps for multiple servers
    static PixelMap3dJson pixelMap3dJson;
};

PixelMap3dJson MonitorOutput3d::pixelMap3dJson;