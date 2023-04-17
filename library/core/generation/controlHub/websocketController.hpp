#pragma once
#include "IHubController.hpp"
#include "controlHub.hpp"
#include "misc/simpleson/json.h"
#include "platform/includes/log.hpp"
#include "platform/includes/thread.hpp"
#include "platform/includes/utils.hpp"
#include "platform/includes/websocketServer.hpp"

class WebsocketController : public IHubController
{
public:
    WebsocketController(ControlHub *hub)
    {
        this->hub = hub;
        // Log::info(TAG, "Created WebsocketController");
        socket = WebsocketServer::createInstance(9800);
        socket->onMessage(handler, (void *)this);
        socket->onConnect(connectionHandler, (void *)this);
    }

    static void connectionHandler(RemoteWebsocketClient *client, WebsocketServer *server, void *userData)
    {
        auto *instance = (WebsocketController *)userData;
        instance->hub->sendCurrentStatus(instance);
    }

    static void handler(RemoteWebsocketClient *client, WebsocketServer *server, std::string msg, void *userData)
    {
        auto *instance = (WebsocketController *)userData;
        json::jobject parsed = json::jobject::parse(msg);
        std::string type = parsed["type"];

        if (type.compare("buttonPressed") == 0)
        {
            int columnIndex = parsed["columnIndex"];
            int slotIndex = parsed["slotIndex"];
            instance->hub->buttonPressed(columnIndex, slotIndex);
        }
        else if (type.compare("buttonReleased") == 0)
        {
            int columnIndex = parsed["columnIndex"];
            int slotIndex = parsed["slotIndex"];
            instance->hub->buttonReleased(columnIndex, slotIndex);
        }
        else if (type.compare("columnDimChange") == 0)
        {
            int columnIndex = parsed["columnIndex"];
            int dim = parsed["dim"];
            instance->hub->dim(columnIndex, dim);
        }
        else if (type.compare("masterDimChange") == 0)
        {
            int dim = parsed["masterDim"];
            instance->hub->setMasterDim(dim);
        }

        else if (type.compare("paramChange") == 0)
        {
            std::string param = parsed["param"];
            float value = ((float)((int)parsed["value"])) / 255.;

            if (param.compare("Amount") == 0)
                instance->hub->setAmount(value);
            else if (param.compare("Intensity") == 0)
                instance->hub->setIntensity(value);
            else if (param.compare("Offset") == 0)
                instance->hub->setOffset(value);
            else if (param.compare("Size") == 0)
                instance->hub->setSize(value);
            else if (param.compare("Variant") == 0)
                instance->hub->setVariant(value);
            else if (param.compare("Velocity") == 0)
                instance->hub->setVelocity(value);
        }
    }

    void onHubSlotActiveChange(int columnIndex, int slotIndex, bool active) override
    {
        socket->sendAll(
            "{\
        \"type\":\"onHubSlotActiveChange\",\
        \"columnIndex\":%d,\
        \"slotIndex\":%d,\
        \"active\":%d\
    }",
            columnIndex,
            slotIndex,
            active);
    }

    void onHubColumnDimChange(int columnIndex, uint8_t dim) override
    {
        socket->sendAll(
            "{\
        \"type\":\"onHubColumnDimChange\",\
        \"columnIndex\":%d,\
        \"dim\":%d\
    }",
            columnIndex,
            dim);
    }

    void onHubMasterDimChange(uint8_t dim) override
    {
        socket->sendAll(
            "{\
        \"type\":\"onHubMasterDimChange\",\
        \"masterDim\":%d\
    }",
            dim);
    }

    void onHubSlotNameChange(int columnIndex, int slotIndex, std::string name) override
    {
        socket->sendAll(
            "{\
        \"type\":\"onHubSlotNameChange\",\
        \"columnIndex\":%d,\
        \"slotIndex\":%d,\
        \"name\":\"%s\"\
    }",
            columnIndex,
            slotIndex,
            name.c_str());
    }
    void onHubColumnNameChange(int columnIndex, std::string name) override
    {
        socket->sendAll(
            "{\
        \"type\":\"onHubColumnNameChange\",\
        \"columnIndex\":%d,\
        \"name\":\"%s\"\
    }",
            columnIndex,
            name.c_str());
    }

    void onHubVelocityChange(float velocity) override { onHubParamChange("Velocity", velocity); };
    void onHubAmountChange(float amount) override { onHubParamChange("Amount", amount); };
    void onHubIntensityChange(float intensity) override { onHubParamChange("Intensity", intensity); };
    void onHubVariantChange(float variant) override { onHubParamChange("Variant", variant); };
    void onHubSizeChange(float size) override { onHubParamChange("Size", size); };
    void onHubOffsetChange(float offset) override { onHubParamChange("Offset", offset); };

private:
    ControlHub *hub;
    std::unique_ptr<WebsocketServer> socket;
    const char *TAG = "WebsocketController";

    void onHubParamChange(const char *param, float value)
    {
        socket->sendAll(
            "{\
        \"type\":\"onHubParamChange\",\
        \"param\":\"%s\",\
        \"value\":\"%d\"\
    }",
            param,
            (int)(value * 255));
    }
};
