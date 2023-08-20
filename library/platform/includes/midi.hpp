#pragma once
#include "platform/includes/midiDevice.hpp"
#include <map>
#include <string>
#include <vector>

class Midi
{
public:
    static Midi *getInstance(){
        if (!instance)
            instance = createInstance();
        return instance;
    }

    using DeviceCreatedDestroyedCallback = void (*)(MidiDevice *device, std::string name, void *userData);

    virtual void onDeviceCreatedDestroyed(DeviceCreatedDestroyedCallback cb_created, DeviceCreatedDestroyedCallback cb_destroyed, void *userData)=0;

    virtual ~Midi() = default;
protected:
    static Midi *instance;
    static Midi *createInstance();
    Midi(){}
};