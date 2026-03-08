#pragma once

#include "core/generation/pixelMap/pixelMap.hpp"
#include "log.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/webServer.hpp"
#include "platform/includes/webServerResponseBuilder.hpp"
#include "websocketOutput.hpp"
#include <stdarg.h>
#include <string>

// MonitorOutput displays the led data in your browser
class MonitorOutput final: public WebsocketOutput
{

    class PixelMapJson : public WebServerResponseBuilder
    {

        void build(WebServerResponseBuilder::Writer writer, void *userData) override
        {
            // Log::info("","Building mapping json. ");
            write(userData, writer, "[\n");
            int outputIndex = 0;
            for (auto &output : outputs)
            {
                write(userData, writer, "  {\n");
                write(userData, writer, "    \"path\": \"/ws/monitor%d\",\n", output.mapIndex);
                write(userData, writer, "    \"instance\": \"%s\",\n", output.instanceName ? output.instanceName : "");
                write(userData, writer, "    \"type\": \"%s\",\n", output.map ? "2d" : "3d");
                write(userData, writer, "    \"size\": %f,\n", output.size);
                write(userData, writer, "    \"positions\": [\n");
                int index = 0;
                if (output.map)
                {
                    for (auto pixel = output.map->begin(); pixel != output.map->end(); pixel++)
                        write(userData, writer, "      {\"x\" : %f, \"y\" : %f}%s \n", pixel->x, pixel->y, index++ == output.map->size() - 1 ? "" : ",");
                }
                else if (output.map3d)
                {
                    for (auto pixel = output.map3d->begin(); pixel != output.map3d->end(); pixel++)
                        write(userData, writer, "      {\"x\" : %f, \"y\" : %f, \"z\" : %f}%s \n", pixel->x, pixel->y, pixel->z, index++ == output.map3d->size() - 1 ? "" : ",");
                }
                write(userData, writer, "    ]\n");
                write(userData, writer, "  }%s\n", outputIndex++ == outputs.size() - 1 ? "" : ",");
            }
            write(userData, writer, "]\n");
        }

        std::vector<PixelMap> maps;

    public:
        unsigned int addOutput(PixelMap *map, const char* instanceName, float size)
        {
            outputs.push_back({.map = map, .mapIndex = ++mapIndex, .size=size, .instanceName = instanceName});
            return mapIndex;
        }

        unsigned int addOutput(PixelMap3d *map, const char* instanceName, float size)
        {
            outputs.push_back({.map3d = map, .mapIndex = ++mapIndex, .size=size, .instanceName = instanceName});
            return mapIndex;
        }

    private:
        struct PixelMonitorOutput
        {
            PixelMap *map = nullptr; //either map or map3d will be used, never both
            PixelMap3d *map3d = nullptr;
            unsigned int mapIndex;
            float size;
            const char* instanceName;
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

public:
    MonitorOutput(WebServer **webServer, PixelMap *map, const char* instanceName=nullptr, unsigned int fps = 60, float size = 0.01) : WebsocketOutput(webServer, pathBuf, fps)
    {
        this->webServer = webServer;
        snprintf(pathBuf, 20, "/ws/monitor%d", pixelMapJson.addOutput(map, instanceName, size));
    }

    MonitorOutput(WebServer **webServer, PixelMap3d *map, const char* instanceName=nullptr, unsigned int fps = 60, float size = 0.01) : WebsocketOutput(webServer, pathBuf, fps)
    {
        this->webServer = webServer;
        snprintf(pathBuf, 20, "/ws/monitor%d", pixelMapJson.addOutput(map, instanceName, size));
    }

    void initialize() override 
    {
        // Log::info(TAG,"beginning MonitorOutput");
        WebsocketOutput::initialize();
        (*webServer)->addPath("/monitor/mapping.json", &pixelMapJson);
    }

private:
    WebServer **webServer;
    char pathBuf[20];
    static PixelMapJson pixelMapJson;
};

MonitorOutput::PixelMapJson MonitorOutput::pixelMapJson;