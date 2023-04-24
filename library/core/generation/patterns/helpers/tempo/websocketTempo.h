#pragma once
#include "abstractTempo.h"
#include "log.hpp"
#include "utils.hpp"
#include "platform/includes/websocketServer.hpp"

class WebsocketTempo : public AbstractTempo, public TempoListener
{

public:
    WebsocketTempo()
    {
        sourceName = "Browser Tap";
        socket= WebsocketServer::createInstance(9799);
        //Todo implement tapping from browser
        //socket->onMessage(handler, (void *)this);
        //socket->onConnect(connectionHandler, (void *)this);

    }

    void OnBeat(int beatNr, const char* sourceName) override {
        socket->sendAll("{\
            \"beatNr\": %d,\
            \"sourceName\": \"%s\"\
        }",
            beatNr, sourceName);
    }

    friend class Tempo;

private:
    std::unique_ptr<WebsocketServer> socket;

    void TempoTask() override {}

    // static void connectionHandler(RemoteWebsocketClient *client, WebsocketServer *server, void *userData)
    // {

    // }

    // static void handler(RemoteWebsocketClient *client, WebsocketServer *server, std::string msg, void *userData)
    // {

    // }
};
