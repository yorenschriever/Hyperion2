#include "midi.hpp"
#include "midi-device.hpp"
#include "midi-device-macos.hpp"
#include "thread.hpp"
#include "log.hpp"
#include "rtmidi/RtMidi.h"

bool Midi::initialized = false;
std::vector<MidiDevice*> Midi::devices;
std::map<std::string,std::string> Midi::name_pairs;
Midi::DeviceCreatedDestroyedCallback Midi::cb_created;
Midi::DeviceCreatedDestroyedCallback Midi::cb_destroyed;
void* Midi::callbackUserData = 0;

void Midi::initialize()
{
    if (initialized)
        return;
    initialized = true;

    //sometimes the name of the input device is not the same as the output device
    //here we watch them
    // order: input name, output name
    name_pairs.insert({"nanoKONTROL2 SLIDER/KNOB", "nanoKONTROL2 CTRL"});

    // all midi interaction should take place from the same thread.
    Thread::create(midiTask, "midiTask", Thread::Purpose::control, 1500, nullptr, 1);
}

void Midi::midiTask(void *param)
{
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

            for (auto device : devices)
            {
                if (cb_destroyed != nullptr)
                {
                    cb_destroyed(device,"", callbackUserData);
                }
                delete device;
            }
            devices.clear();

            int min_out_index = 0;

            typedef struct{
                int index;
                std::string name;
            } Index_Name;

            std::vector<Index_Name> in_list;
            for (int i = 0; i < midiInCount; i++)
                in_list.push_back({.index=i, .name=midiInPoll.getPortName(i)});

            std::vector<Index_Name> out_list;
            for (int i = 0; i < midiOutCount; i++)
                out_list.push_back({.index=i, .name=midiOutPoll.getPortName(i)});

            for(auto in: in_list){
                auto match = std::find_if(
                    out_list.begin(),
                     out_list.end(), 
                     [in] (const Index_Name& s) { 
                        if (s.name.compare(in.name)==0)
                            return true;
                        return s.name.compare(name_pairs[in.name])==0;
                    }
                );
                if (match != out_list.end()){
                    createDevice(in.index, match[0].index, in.name);
                    out_list.erase(match);     
                } else {
                    createDevice(in.index, -1, in.name);
                }
            }
            
            for(auto out: out_list){
                createDevice(-1, out.index, out.name);
            }
        }

        Thread::sleep(200);
    }
}


void Midi::createDevice(int in_index, int out_index, std::string name)
{
    Log::info("MIDI","Created device for %d,%d, %s", in_index,out_index,name.c_str());

    auto midiDevice = new MidiDeviceMacos(in_index,out_index); 

    devices.push_back(midiDevice );

    if (cb_created != nullptr)
    {
        cb_created(midiDevice,name, callbackUserData);
    }
}

void Midi::onDeviceCreatedDestroyed( DeviceCreatedDestroyedCallback cb_created, DeviceCreatedDestroyedCallback cb_destroyed, void* userData)
{
    Midi::cb_created = cb_created;
    Midi::cb_destroyed = cb_destroyed;
    Midi::callbackUserData = userData;
}