#pragma once
#include "IHubController.hpp"
#include "controlHub.hpp"
#include "misc/cjson/cJSON.h"
#include "platform/includes/log.hpp"
#include "platform/includes/thread.hpp"
#include "platform/includes/utils.hpp"
#include "platform/includes/websocketServer.hpp"
#include "platform/includes/webServer.hpp"
#include "build.hpp"
class WebsocketController : public IHubController
{
public:
    WebsocketController(ControlHub *hub, WebServer *server)
    {
        this->hub = hub;
        // Log::info(TAG, "Created WebsocketController");
        socket = WebsocketServer::createInstance(server,"/ws/controller");
        socket->onMessage(handler, (void *)this);
        socket->onConnect(connectionHandler, (void *)this);
    }

    static void connectionHandler(RemoteWebsocketClient *client, WebsocketServer *server, void *userData)
    {
        auto *instance = (WebsocketController *)userData;
        instance->sendBuildId();
        instance->hub->sendCurrentStatus(instance);
    }

    static void handler(RemoteWebsocketClient *client, WebsocketServer *server, std::string msg, void *userData)
    {
        auto *instance = (WebsocketController *)userData;
        cJSON *parsed = cJSON_Parse(msg.c_str());
        std::string type = cJSON_GetObjectItem(parsed,"type")->valuestring;

        if (type.compare("buttonPressed") == 0)
        {
            int columnIndex = cJSON_GetObjectItem(parsed,"columnIndex")->valueint;
            int slotIndex = cJSON_GetObjectItem(parsed,"slotIndex")->valueint;
            instance->hub->buttonPressed(columnIndex, slotIndex);
        }
        else if (type.compare("buttonReleased") == 0)
        {
            int columnIndex = cJSON_GetObjectItem(parsed,"columnIndex")->valueint;
            int slotIndex = cJSON_GetObjectItem(parsed,"slotIndex")->valueint;
            instance->hub->buttonReleased(columnIndex, slotIndex);
        }
        else if (type.compare("columnDimChange") == 0)
        {
            int columnIndex = cJSON_GetObjectItem(parsed,"columnIndex")->valueint;
            int dim = cJSON_GetObjectItem(parsed,"dim")->valueint;
            instance->hub->dim(columnIndex, dim);
        }
        else if (type.compare("masterDimChange") == 0)
        {
            int dim = cJSON_GetObjectItem(parsed,"masterDim")->valueint;
            instance->hub->setMasterDim(dim);
        }

        else if (type.compare("paramChange") == 0)
        {
            std::string param = cJSON_GetObjectItem(parsed,"param")->valuestring;
            float value = cJSON_GetObjectItem(parsed,"value")->valuedouble / 255.;

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

        cJSON_Delete(parsed);
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

    // This method sends a string to the client that identifies the code that is running here.
    // This information is used when a websocket reconnects: if the build id is the same, 
    // we are likely dealing with a restored connection, but if the build id is different
    // we are likely dealing with a new iteration of a dev build. This resets and rebuilds the
    // internal state of the client so everything is guaranteed to be in sync with the new version.
    void sendBuildId()
    {
        socket->sendAll(
            "{\
        \"type\":\"buildId\",\
        \"value\":\"%s\"\
    }",
        Build::getBuildId());
    }
};
