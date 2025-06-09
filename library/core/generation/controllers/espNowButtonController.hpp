#pragma once

#include "core/generation/controlHub/IHubController.hpp"
#include "core/generation/controlHub/controlHub.hpp"
#include "log.hpp"
#include "midiController.hpp"
#include "midiDevice.hpp"
#include <algorithm>

class EspNowButtonController : public MidiController
{
private:
    ControlHub *hub;
    MidiDevice *midi;
    int columnIndex = 0; 

    const char* TAG = "ESP-NOW-BUTTON";

    const int MIDI_CHANNEL = 15;

public:
    EspNowButtonController(ControlHub *hub, MidiDevice *midi, int columnIndex = 0)
    {
        this->hub = hub;
        this->midi = midi;
        this->columnIndex = columnIndex;

        hub->subscribe(this);
        midi->addMidiListener(this);

        // Log::info(TAG, "constructor");
    }

    ~EspNowButtonController(){
        hub->unsubscribe(this);
        midi->removeMidiListener(this);

        // Log::info(TAG, "destructor");
    }

    void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) override {
        // Log::info(TAG, "note on %d %d", note, velocity);
        hub->buttonPressed(columnIndex, note);
    }
    void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) override {
        // Log::info(TAG, "note on %d %d", note, velocity);
        hub->buttonReleased(columnIndex, note);
    }

    void onHubSlotActiveChange(int columnIndex, int slotIndex, bool active) override {}

    void onControllerChange(uint8_t channel, uint8_t controller, uint8_t value) override {}
    void onSystemRealtime(uint8_t message) override {}

};
