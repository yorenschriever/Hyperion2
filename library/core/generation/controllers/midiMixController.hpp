#pragma once

#include "core/generation/controlHub/IHubController.hpp"
#include "core/generation/controlHub/controlHub.hpp"
#include "log.hpp"
#include "midiController.hpp"
#include "platform/includes/midiDevice.hpp"
#include <algorithm>

class MidiMixController : public MidiController
{
protected:
    ControlHub *hub;
    MidiDevice *midi;

    const char* TAG = "MIDIMIX";

    const int MIDI_CHANNEL = 0;

public:
    MidiMixController(ControlHub *hub, MidiDevice *midi)
    {
        this->hub = hub;
        this->midi = midi;

        hub->subscribe(this);
        midi->addMidiListener(this);

        //Log::info(TAG, "constructor");
    }

    ~MidiMixController(){
        hub->unsubscribe(this);
        midi->removeMidiListener(this);

        //Log::info(TAG, "destructor");
    }

    void onControllerChange(uint8_t channel, uint8_t controller, uint8_t value) override
    {
        //Log::info(TAG, "onControllerChange %d %d", controller, value);

        if (controller == 16)
            return hub->setVelocity(0,scale127toFloat(value));
        if (controller == 20)
            return hub->setAmount(0,scale127toFloat(value));
        if (controller == 24)
            return hub->setSize(0,scale127toFloat(value));
        if (controller == 28)
            return hub->setOffset(0,scale127toFloat(value));
        if (controller == 46)
            return hub->setVariant(0,scale127toFloat(value));
        if (controller == 50)
            return hub->setIntensity(0,scale127toFloat(value));
    }
    void onSystemRealtime(uint8_t message) override {}
    void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) override {}
    void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) override {}
};
