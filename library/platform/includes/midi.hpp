#pragma once
#include "platform/includes/midi-device.hpp"
#include <map>
#include <string>
#include <vector>

class Midi
{
    using DeviceCreatedDestroyedCallback = void (*)(MidiDevice *device, std::string name, void *userData);

public:
    static void initialize();
    static void onDeviceCreatedDestroyed(DeviceCreatedDestroyedCallback cb_created, DeviceCreatedDestroyedCallback cb_destroyed, void *userData);

private:
    static bool initialized;
    static std::vector<MidiDevice *> devices;
    static std::map<std::string, std::string> name_pairs;

    static void midiTask(void *param);
    static void createDevice(int in_index, int out_index, std::string name);
    static DeviceCreatedDestroyedCallback cb_created, cb_destroyed;
    static void *callbackUserData;
};