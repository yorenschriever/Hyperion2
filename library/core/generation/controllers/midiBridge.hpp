#pragma once
#include "platform/includes/midi.hpp"
#include "platform/includes/midiDevice.hpp"
#include "platform/includes/websocketServer.hpp"
#include "platform/includes/webServer.hpp"
#include "misc/cjson/cJSON.h"
#include <string>
#include <map>
#include "log.hpp"
#include "midiListener.hpp"
#include <mutex>

struct DeviceCreatedDestroyedEntry
{
  Midi::DeviceCreatedDestroyedCallback created;
  Midi::DeviceCreatedDestroyedCallback destroyed;
  void *userData;
};

class IMidiBridgeSender {
    public:
    virtual void sendMidiMessage(RemoteWebsocketClient *client, int deviceId, const unsigned char* msg, int size)=0;
};

class MidiBridgeDevice : public MidiDevice
{
public:
  std::string name;

  MidiBridgeDevice(IMidiBridgeSender * bridge, RemoteWebsocketClient *client, int deviceId, std::string name)
  {
    this->bridge = bridge;
    this->client = client;
    this->deviceId = deviceId;
    this->name = name;
  }

  void sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) override
  {
    const unsigned char data[3] = {(unsigned char)(MIDI_NOTE_ON | channel), note, velocity};
    bridge->sendMidiMessage(client, deviceId, data, sizeof(data));
  }

  void sendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) override
  {
    const unsigned char data[3] = {(unsigned char)(MIDI_NOTE_OFF | channel), note, velocity};
    bridge->sendMidiMessage(client, deviceId, data, sizeof(data));
  }

  void sendControllerChange(uint8_t channel, uint8_t controller, uint8_t value) override
  {
    const unsigned char data[3] = {(unsigned char)(MIDI_CONTROLLER_CHANGE | channel), controller, value};
    bridge->sendMidiMessage(client, deviceId, data, sizeof(data));
  }

  bool waitTxDone(int timeout = 500) override
  {
    return true; // this is not implemented
  };

  bool isConnected() override
  {
    return true; // TODO?
  }

  void handleMessage(std::vector<unsigned char> *message)
  {
    // Log::info("MIDIBRIDGEMSG","handleMessage: %d, %x %x %x", message->size(), message->at(0),message->at(1),message->at(2));

    if (listeners.size()==0)
      return;

    if (message->size() == 1 && message->at(0) >= 0xF8)
    {
      for (auto listener: listeners)
        listener->onSystemRealtime(message->at(0));
      return;
    }

    if (message->size() == 3)
    {

      uint8_t channel = message->at(0) & 0x0F;
      uint8_t message_type = message->at(0) & 0xF0;

      if (message_type == MIDI_NOTE_ON && message->at(1) < MIDI_NUMBER_OF_NOTES)
      {
        for (auto listener: listeners)
          listener->onNoteOn(channel, message->at(1), message->at(2));
        return;
      }

      if (message_type == MIDI_NOTE_OFF && message->at(1) < MIDI_NUMBER_OF_NOTES)
      {
        for (auto listener: listeners)
          listener->onNoteOff(channel, message->at(1), message->at(2));
        return;
      }

      if (message_type == MIDI_CONTROLLER_CHANGE && message->at(1) < MIDI_NUMBER_OF_CONTROLLERS)
      {
        for (auto listener: listeners)
          listener->onControllerChange(channel, message->at(1), message->at(2));
        return;
      }
    }

    Log::error("MIDI", "Unknown midi bridge message. size=%d, byte0=%x",message->size(),message->at(0));
  }

private:
  IMidiBridgeSender * bridge; 
  int deviceId;
  RemoteWebsocketClient *client;
};


class MidiBridge : IMidiBridgeSender {
private:
    std::map<RemoteWebsocketClient *, std::map<int,MidiBridgeDevice *>> devices;
    std::vector<DeviceCreatedDestroyedEntry> callbacks;
    std::unique_ptr<WebsocketServer> socket;
    const char *TAG = "MidiBridge";
    
public: 
    MidiBridge(WebServer *server){
        socket = WebsocketServer::createInstance(server,"/ws/midiBridge");
        socket->onMessage(handler, (void *)this);
        socket->onConnect(connectionHandler, (void *)this);
        socket->onDisconnect(disconnectionHandler, (void *)this);
    }

    static void connectionHandler(RemoteWebsocketClient *client, WebsocketServer *server, void *userData)
    {
        auto *instance = (MidiBridge *)userData;
        instance->devices[client] = {};
    }

    static void disconnectionHandler(RemoteWebsocketClient *client, WebsocketServer *server, void *userData)
    {
        auto *instance = (MidiBridge *)userData;
        instance->deleteClient(client);
    }

    void onDeviceCreatedDestroyed(Midi::DeviceCreatedDestroyedCallback cb_created, Midi::DeviceCreatedDestroyedCallback cb_destroyed, void *userData){
        callbacks.push_back({.created = cb_created,
                        .destroyed = cb_destroyed,
                        .userData = userData});

        for (auto deviceClients : devices){
            for (auto deviceEntry : deviceClients.second)
            {
                auto device = deviceEntry.second;
                cb_created(device, device->name, userData);
            }
        }
    }

    static void handler(RemoteWebsocketClient *client, WebsocketServer *server, std::string msg, void *userData)
    {
        auto *instance = (MidiBridge *)userData;
        cJSON *parsed = cJSON_Parse(msg.c_str());
        std::string type = cJSON_GetObjectItem(parsed,"type")->valuestring;

        // Log::info(instance->TAG, "ws msg: %s", type.c_str());

        if (type.compare("connected") == 0)
        {
            int deviceId = cJSON_GetObjectItem(parsed,"deviceId")->valueint;
            std::string name = cJSON_GetObjectItem(parsed,"name")->valuestring;

            instance->createDevice(client, deviceId, name);
        }

        if (type.compare("disconnected") == 0)
        {
            int deviceId = cJSON_GetObjectItem(parsed,"deviceId")->valueint;
            instance->deleteDevice(client, deviceId);
        }

        if (type.compare("message") == 0)
        {
            int deviceId = cJSON_GetObjectItem(parsed,"deviceId")->valueint;
            std::string message = cJSON_GetObjectItem(parsed,"message")->valuestring;
            auto device = instance->getDevice(client, deviceId);
            if (device){
                auto parsedMessage = instance->hexStringToBytes(message);
                device->handleMessage(&parsedMessage);
            }
        }

        cJSON_Delete(parsed);
    }


    void createDevice(RemoteWebsocketClient *client, int deviceId, std::string name)
    {
        // Log::info(TAG, "Creating midi bridge device for %s (%d) in client %d", name.c_str(), deviceId, client);
        if (devices.count(client)==0)
            return;

        auto midiDevice = new MidiBridgeDevice(this, client, deviceId, name);

        devices[client].insert({deviceId, midiDevice});

        for (auto cb : callbacks)
            cb.created(midiDevice, name, cb.userData);
    }

    void deleteClient(RemoteWebsocketClient *client)
    {
        if (devices.count(client)==0)
            return;

        for (auto device_entry : devices[client]){
            for (auto cb : callbacks)
            {
                auto device = device_entry.second;
                cb.destroyed(device, device->name, cb.userData);
                delete device;
            }
        }
        devices.erase(client);
    }

    void deleteDevice(RemoteWebsocketClient *client, int deviceId)
    {
        auto device = getDevice(client, deviceId);
        if (!device)
            return;

        // Log::info(TAG, "Deleting midi bridge device for %s (%d) from client %d", device->name.c_str(), deviceId, client);

        for (auto cb : callbacks)
            cb.destroyed(device, device->name, cb.userData);
        
        devices[client].erase(deviceId);

        delete device;
    }

    MidiBridgeDevice * getDevice(RemoteWebsocketClient *client, int deviceId)
    {
        if (devices.count(client)==0)
            return nullptr;
        if (devices[client].count(deviceId)==0)
            return nullptr;
        return devices[client][deviceId];
    }

    void sendMidiMessage(RemoteWebsocketClient *client, int deviceId, const unsigned char* msg, int size) override
    {
        socket->send(client,
            "{\
        \"type\":\"message\",\
        \"deviceId\":%d,\
        \"message\":\"%02x%02x%02x\"\
    }",
            deviceId,
            //in this implementation the length is always 3.
            msg[0], msg[1], msg[2]
        );
    }

private:
    std::vector<unsigned char> hexStringToBytes(const std::string& hex) {
        std::vector<unsigned char> bytes;

        for (unsigned int i = 0; i < hex.length(); i += 2) {
            std::string byteString = hex.substr(i, 2);
            char byte = (char) strtol(byteString.c_str(), NULL, 16);
            bytes.push_back(byte);
        }

        return bytes;
    }
};