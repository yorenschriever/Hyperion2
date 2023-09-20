#pragma once
#include "log.hpp"
#include "midi.hpp"
#include "midiDevice-esp.hpp"

class MidiEsp : public Midi
{

private:
    bool initialized = false;
    std::vector<MidiDeviceEsp *> devices;
    std::vector<DeviceCreatedDestroyedEntry> callbacks;

public:
    MidiEsp()
    {
        initialize();
    }

    ~MidiEsp() = default;

    void initialize()
    {
        if (initialized)
            return;
        initialized=true;
        
        //create midi device on uart2 and connect to the usb to midi converter pins
        createDevice(UART_NUM_2,3,1);

        //create a midi device on uart2 and connect to the midi din connection pins
        //createDevice(UART_NUM_2,35,5);
    }

    void onDeviceCreatedDestroyed(DeviceCreatedDestroyedCallback cb_created, DeviceCreatedDestroyedCallback cb_destroyed, void *userData) override
    {
        callbacks.push_back({.created = cb_created,
                             .destroyed = cb_destroyed,
                             .userData = userData});

        for (auto device : devices){
            cb_created(device, device->name, userData);
        }
    }

private:

    void createDevice(uart_port_t uart, int rxpin, int txpin)
    {
        Log::info("MIDI", "Creating device for uart %d (rx=%d, tx=%d)", uart,rxpin,txpin);

        auto midiDevice = new MidiDeviceEsp(uart,rxpin,txpin, &callbacks);

        devices.push_back(midiDevice);

        //initialize in connected state, even though we did receive anything. 
        //This is for hardware that does not send active sense: we will still see the device as connected
        // NB: only hyperion hardware with forced active sense is supported for now
        // midiDevice->connected = true;
        // for (auto cb : callbacks)
        // {
        //     cb.created(midiDevice, midiDevice->name, cb.userData);
        // }
    
    }
};