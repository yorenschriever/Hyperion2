#pragma once

#include "core/generation/pixelMap.hpp"
#include "log.hpp"
#include "output.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/webServer.hpp"
#include "platform/includes/webServerResponseBuilder.hpp"
#include "websocketOutput.hpp"
#include <stdarg.h>

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
            write(userData, writer, "    \"port\": %d,\n", output.port);
            write(userData, writer, "    \"positions\": [\n");
            int index = 0;
            for (auto pixel : output.map)
                write(userData, writer, "      {\"x\" : %f, \"y\" : %f}%s \n", pixel.x, pixel.y, index++ == output.map.size() - 1 ? "" : ",");
            write(userData, writer, "    ]\n");
            write(userData, writer, "  }%s\n", outputIndex++ == outputs.size() - 1 ? "" : ",");
        }
        write(userData, writer, "]\n");
    }

    std::vector<PixelMap> maps;

public:
    unsigned int addOutput(PixelMap map)
    {
        outputs.push_back({.map = map, .port = ++port});
        return port;
    }

private:
    struct PixelMonitorOutput
    {
        PixelMap map;
        unsigned int port;
    };

    std::vector<PixelMonitorOutput> outputs;
    bool begun;
    unsigned int port = 9750;
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
class MonitorOutput : public WebsocketOutput
{
public:
    MonitorOutput(PixelMap map, unsigned int fps = 60) : WebsocketOutput(0, fps)
    {
        this->port = pixelMapJson.addOutput(map);
    }

    void begin() override
    {
        // Log::info(TAG,"beginning MonitorOutput on port %d",port);
        WebsocketOutput::begin();
    }

    static void addPathToServer(WebServer *server)
    {
        server->addPath("/monitor2d/mapping.json", &pixelMapJson);
    }

private:
    // todo support multiple servers / pixel maps
    static PixelMapJson pixelMapJson;
};

PixelMapJson MonitorOutput::pixelMapJson;