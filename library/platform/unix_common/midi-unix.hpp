#include "log.hpp"
#include "midi.hpp"
#include "midiDevice-unix.hpp"
#include "midiDevice.hpp"
#include "rtmidi/RtMidi.h"
#include "thread.hpp"
#include <algorithm>

class MidiUnix : public Midi
{

private:
    struct DeviceCreatedDestroyedEntry
    {
        Midi::DeviceCreatedDestroyedCallback created;
        DeviceCreatedDestroyedCallback destroyed;
        void *userData;
    };

    bool initialized = false;
    std::vector<MidiDevice *> devices;
    std::map<std::string, std::string> name_pairs;
    std::vector<DeviceCreatedDestroyedEntry> callbacks;

    std::string trimDeviceName(std::string s)
    {
        std::string::size_type pos = s.find(':');
        if (pos == std::string::npos)
            return s;

        return s.substr(0, pos);
    }

public:
    MidiUnix(){
        initialize();
    }

    ~MidiUnix() = default;

    void initialize()
    {
        if (initialized)
            return;
        initialized = true;

        // sometimes the name of the input device is not the same as the output device
        // here we watch them
        // order: input name, output name
        name_pairs.insert({"nanoKONTROL2 SLIDER/KNOB", "nanoKONTROL2 CTRL"});

        // all midi interaction should take place from the same thread.
        Thread::create(midiTask, "midiTask", Thread::Purpose::control, 1500, this, 1);
    }

    void onDeviceCreatedDestroyed(DeviceCreatedDestroyedCallback cb_created, DeviceCreatedDestroyedCallback cb_destroyed, void *userData) override
    {
        callbacks.push_back({.created = cb_created,
                             .destroyed = cb_destroyed,
                             .userData = userData});
    }

private:
    static void midiTask(void *param)
    {
        MidiUnix *instance = (MidiUnix *)param;
        // instances used to pull the number of connected devices.
        // once this number changes, i disconnect everything and build it all up again
        RtMidiIn midiInPoll;
        RtMidiOut midiOutPoll;
        int midiInCount = 0;
        int midiOutCount = 0;

        while (true)
        {
            int newMidiInCount = midiInPoll.getPortCount();
            int newMidiOutCount = midiOutPoll.getPortCount();

            if (newMidiInCount != midiInCount || newMidiOutCount != midiOutCount)
            {
                midiInCount = newMidiInCount;
                midiOutCount = newMidiOutCount;

                for (auto device : instance->devices)
                {
                    for (auto cb : instance->callbacks)
                    {
                        cb.destroyed(device, "", cb.userData);
                    }
                    delete device;
                }
                instance->devices.clear();

                int min_out_index = 0;

                typedef struct
                {
                    int index;
                    std::string name;
                } Index_Name;

                std::vector<Index_Name> in_list;
                for (int i = 0; i < midiInCount; i++)
                {
                    std::string name = midiInPoll.getPortName(i);
                    if (name.size() == 0)
                        continue;
                    in_list.push_back({.index = i, .name = name});
                }

                std::vector<Index_Name> out_list;
                for (int i = 0; i < midiOutCount; i++)
                {
                    std::string name = midiInPoll.getPortName(i);
                    if (name.size() == 0)
                        continue;
                    out_list.push_back({.index = i, .name = name});
                }

                for (auto in : in_list)
                {
                    auto match = std::find_if(
                        out_list.begin(),
                        out_list.end(),
                        [in, instance](const Index_Name &s)
                        {
                            if (s.name.compare(in.name) == 0)
                                return true;
                            return s.name.compare(instance->name_pairs[in.name]) == 0;
                        });
                    if (match != out_list.end())
                    {
                        instance->createDevice(in.index, match[0].index, in.name);
                        out_list.erase(match);
                    }
                    else
                    {
                        instance->createDevice(in.index, -1, in.name);
                    }
                }

                for (auto out : out_list)
                {
                    instance->createDevice(-1, out.index, out.name);
                }
            }

            Thread::sleep(200);
        }
    }

    void createDevice(int in_index, int out_index, std::string name)
    {
        Log::info("MIDI", "Creating device for %d,%d, %s", in_index, out_index, name.c_str());

#if TRIM_MIDI_DEVICE_NAME
        name = trimDeviceName(name);
        Log::info("MIDI", "trimmed midi device name to %s", name.c_str());
#endif

        auto midiDevice = new MidiDeviceUnix(in_index, out_index);

        devices.push_back(midiDevice);

        for (auto cb : callbacks)
        {
            cb.created(midiDevice, name, cb.userData);
        }
    }
};