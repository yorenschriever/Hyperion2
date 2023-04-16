#pragma once
#include "IHubController.hpp"
#include "controlHub.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/thread.hpp"
#include "platform/includes/utils.hpp"
#include "platform/includes/websocketServer.hpp"
#include "misc/simpleson/json.h"

class WebsocketController : public IHubController
{
public:
    WebsocketController(ControlHub *hub)
    {
        this->hub=hub;
        //Log::info(TAG, "Created WebsocketController");
        socket = WebsocketServer::createInstance(9800);
        socket->onMessage(handler, (void*) this);
        socket->onConnect(connectionHandler, (void*) this);
    }

    static void connectionHandler(RemoteWebsocketClient *client, WebsocketServer *server, void* userData)
    {
        auto *instance = (WebsocketController*) userData;
        instance->hub->sendCurrentStatus(instance);
    }

    static void handler(RemoteWebsocketClient *client, WebsocketServer *server, std::string msg, void* userData)
    {
        auto *instance = (WebsocketController*) userData;
        json::jobject parsed = json::jobject::parse(msg);
        std::string type = parsed["type"];
        int columnIndex = parsed["columnIndex"];
        int slotIndex = parsed["slotIndex"];
        //Log::info("WebsocketController", "handling message: ", type.c_str());
        //Log::info("WebsocketController", "columnIndex %d, slotIndex:%d ", columnIndex, slotIndex);

        if (type.compare("buttonPressed") == 0){
            instance->hub->buttonPressed(columnIndex,slotIndex);
        } else if (type.compare("buttonReleased") == 0) {
            instance->hub->buttonReleased(columnIndex,slotIndex);
        }
    }

    void onHubSlotActiveChange(int columnIndex, int slotIndex, bool active) override
    {
        socket->sendAll("{\
        \"type\":\"onHubSlotActiveChange\",\
        \"columnIndex\":%d,\
        \"slotIndex\":%d,\
        \"active\":%d\
    }",
                        columnIndex, slotIndex, active);
    }

    void onHubColumnDimChange(int columnIndex, uint8_t dim) override
    {
        socket->sendAll("{\
        \"type\":\"onHubColumnDimChange\",\
        \"columnIndex\":%d,\
        \"dim\":%d\
    }",
                        columnIndex, dim);
    }

    void onHubMasterDimChange(uint8_t dim) override
    {
        socket->sendAll("{\
        \"type\":\"onHubMasterDimChange\",\
        \"masterDim\":%d\
    }",
                        dim);
    }


  void onHubSlotNameChange(int columnIndex, int slotIndex, std::string name) override {
        socket->sendAll("{\
        \"type\":\"onHubSlotNameChange\",\
        \"columnIndex\":%d,\
        \"slotIndex\":%d,\
        \"name\":\"%s\"\
    }",
                        columnIndex, slotIndex, name.c_str());
  }
  void onHubColumnNameChange(int columnIndex, std::string name) override {
        socket->sendAll("{\
        \"type\":\"onHubColumnNameChange\",\
        \"columnIndex\":%d,\
        \"name\":\"%s\"\
    }",
                        columnIndex, name.c_str());
  }

private:
    ControlHub *hub;
    std::unique_ptr<WebsocketServer> socket;
    const char *TAG = "WebsocketController";
};
