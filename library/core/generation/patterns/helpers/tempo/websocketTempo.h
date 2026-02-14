#pragma once
#include "abstractTempo.h"
#include "tapTempo.h"
#include "log.hpp"
#include "platform/includes/webServer.hpp"
#include "platform/includes/websocketServer.hpp"
#include "utils.hpp"

class WebsocketTempo : public AbstractTempo, public TempoListener
{

public:
    WebsocketTempo(WebServer *server)
    {
        sourceName = "Browser Tap";
        socket = WebsocketServer::createInstance(server, "/ws/tempo");
        socket->onMessage(handler, (void *)this);
    }

    void OnBeat(int beatNr, const char *sourceName) override
    {
            socket->sendAll(
                "{\
            \"beatNr\": %d,\
            \"sourceName\": \"%s\"\
        }",
                beatNr, sourceName);
        lastBeatSent = Utils::millis();
    }

    static void handler(RemoteWebsocketClient *client, WebsocketServer *server, std::string msg, void *userData)
    {
        cJSON *parsed = cJSON_Parse(msg.c_str());
        std::string type = cJSON_GetObjectItem(parsed,"type")->valuestring;

        if (type.compare("tap") == 0)
            TapTempo::getInstance()->Tap();
        if (type.compare("stop") == 0)
            TapTempo::getInstance()->Stop();
        if (type.compare("align") == 0)
            TapTempo::getInstance()->Align();
        if (type.compare("alignPhrase") == 0)
            Tempo::AlignPhrase();

        cJSON_Delete(parsed);
    }

    friend class Tempo;

private:
    std::unique_ptr<WebsocketServer> socket;

    unsigned long lastBeatSent = 0;
    void TempoTask() override
    {
        if (Utils::millis() - lastBeatSent > 1000)
        {
            OnBeat(Tempo::GetBeatNumber(), Tempo::SourceName());
        }
    }

    // static void connectionHandler(RemoteWebsocketClient *client, WebsocketServer *server, void *userData)
    // {

    // }

    // static void handler(RemoteWebsocketClient *client, WebsocketServer *server, std::string msg, void *userData)
    // {

    // }
};
