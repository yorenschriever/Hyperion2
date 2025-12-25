#pragma once

#include "core/generation/pixelMap.hpp"
#include "log.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/webServer.hpp"
#include "platform/includes/webServerResponseBuilder.hpp"
#include "websocketOutput.hpp"
#include <stdarg.h>
#include <string>

class PixelMapJson : public WebServerResponseBuilder
{

    void build(WebServerResponseBuilder::Writer writer, void *userData) override
    {
        // Log::info("","Building mapping json. ");
        write(userData, writer, "[\n");
        int outputIndex = 0;
        for (auto output : outputs)
        {
            write(userData, writer, "  {\n");
            write(userData, writer, "    \"path\": \"/ws/monitor%d\",\n", output.mapIndex);
            write(userData, writer, "    \"size\": %f,\n", output.size);
            write(userData, writer, "    \"positions\": [\n");
            int index = 0;
            for (auto pixel = output.map->begin(); pixel != output.map->end(); pixel++)
                write(userData, writer, "      {\"x\" : %f, \"y\" : %f}%s \n", pixel->x, pixel->y, index++ == output.map->size() - 1 ? "" : ",");
            write(userData, writer, "    ]\n");
            write(userData, writer, "  }%s\n", outputIndex++ == outputs.size() - 1 ? "" : ",");
        }
        write(userData, writer, "]\n");
    }

    std::vector<PixelMap> maps;

public:
    unsigned int addOutput(PixelMap *map, float size)
    {
        outputs.push_back({.map = map, .mapIndex = ++mapIndex, .size=size});
        return mapIndex;
    }

private:
    struct PixelMonitorOutput
    {
        PixelMap *map;
        unsigned int mapIndex;
        float size;
    };

    std::vector<PixelMonitorOutput> outputs;
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

// MonitorOutput displays the led data in your browser
class MonitorOutput final: public WebsocketOutput
{
public:
    MonitorOutput(WebServer **webServer, PixelMap *map, unsigned int fps = 60, float size = 0.01) : WebsocketOutput(webServer, pathBuf, fps)
    {
        this->webServer = webServer;
        snprintf(pathBuf, 20, "/ws/monitor%d", pixelMapJson.addOutput(map, size));
    }

    void initialize() override 
    {
        // Log::info(TAG,"beginning MonitorOutput");
        WebsocketOutput::initialize();
        (*webServer)->addPath("/monitor2d/mapping.json", &pixelMapJson);
    }

private:
    WebServer **webServer;
    char pathBuf[20];
    // todo support multiple pixel maps for multiple servers
    static PixelMapJson pixelMapJson;
};

PixelMapJson MonitorOutput::pixelMapJson;