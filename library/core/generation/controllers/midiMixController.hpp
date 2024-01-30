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
        if (controller < 32)
            controller -= 16;
        else
            controller -= (42 - 12);

        int col = (controller) /4;
        int row = (controller) % 4;

        if (col >= 8) return;

        // Log::info(TAG, "onControllerChange %d %d = %d", col, row, value);

        if (row==3)
            return hub->setVelocity(col,scale127toFloat(value));
        if (row==2)
            return hub->setAmount(col,scale127toFloat(value));
        if (row==1)
            return hub->setSize(col,scale127toFloat(value));
        if (row==0)
            return hub->setOffset(col,scale127toFloat(value));

    }
    void onSystemRealtime(uint8_t message) override {}
    void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) override {}
    void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) override {}
};
