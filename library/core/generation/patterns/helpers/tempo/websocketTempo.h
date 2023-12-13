#pragma once
#include "abstractTempo.h"
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
        // Todo implement tapping from browser
        // socket->onMessage(handler, (void *)this);
        // socket->onConnect(connectionHandler, (void *)this);
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
