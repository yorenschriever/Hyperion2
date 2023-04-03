#include "midi.hpp"
#include "log.hpp"
#include "midi-device.hpp"
#include "thread.hpp"

bool Midi::initialized = false;
std::vector<MidiDevice *> Midi::devices;
std::map<std::string, std::string> Midi::name_pairs;
std::vector<Midi::DeviceCreatedDestroyedEntry> Midi::callbacks;

void Midi::initialize()
{
    if (initialized)
        return;
    initialized = true;
}

void Midi::midiTask(void *param)
{
}

void Midi::createDevice(int in_index, int out_index, std::string name)
{
}

void Midi::onDeviceCreatedDestroyed(DeviceCreatedDestroyedCallback cb_created, DeviceCreatedDestroyedCallback cb_destroyed, void *userData)
{
}