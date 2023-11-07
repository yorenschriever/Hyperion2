#pragma once
#include "core/generation/controllers/midiControllerFactory.hpp"
#include "core/generation/controllers/midiMixController.hpp"
#include "core/generation/controlHub/IHubController.hpp"
#include "core/generation/controlHub/controlHub.hpp"
#include "log.hpp"
#include "midiController.hpp"
#include "platform/includes/midiDevice.hpp"
#include <algorithm>

class Corrections {
    public:
    static int corrections[6];
};

int Corrections::corrections[6];

class CustomMidiMixController : public MidiMixController
{
public:
    CustomMidiMixController(ControlHub *hub, MidiDevice *midi) : MidiMixController(hub, midi){}

    void onControllerChange(uint8_t channel, uint8_t controller, uint8_t value) override
    {
        //Log::info(TAG, "custom onControllerChange %d %d", controller, value);

        bool log=true;
        if (controller == 18)
            Corrections::corrections[0] = value-64;
        else if (controller == 22)
            Corrections::corrections[1] = value-64;
        else if (controller == 26)
            Corrections::corrections[2] = value-64;
        else if (controller == 30)
            Corrections::corrections[3] = value-64;
        else if (controller == 48)
            Corrections::corrections[4] = value-64;
        else if (controller == 52)
            Corrections::corrections[5] = value-64;
        else
            log=false;

        if (log)
        Log::info(TAG, "Corrections: %d, %d, %d, %d, %d, %d",
            Corrections::corrections[0],
            Corrections::corrections[1],
            Corrections::corrections[2],
            Corrections::corrections[3],
            Corrections::corrections[4],
            Corrections::corrections[5]);

        MidiMixController::onControllerChange(channel, controller, value);
    }

};


class CustomMidiControllerFactory : public MidiControllerFactory
{
public:
  // override the method that determines which controller to pick for a given device
  virtual std::unique_ptr<MidiController> create(MidiDevice *device, std::string name, ControlHub *hub) override
  {
    if (name.compare("MIDI Mix")==0)
        return std::unique_ptr<CustomMidiMixController>(new CustomMidiMixController(hub,device));

    // otherwise, return the value that the original MidiControllerFactory would have returned
    return MidiControllerFactory::create(device, name, hub);
  }
};