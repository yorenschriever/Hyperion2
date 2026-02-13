#pragma once

#include "core/generation/controlHub/IHubController.hpp"
#include "core/generation/controlHub/controlHub.hpp"
#include "log.hpp"
#include "midiController.hpp"
#include "midiDevice.hpp"
#include <algorithm>
#include <vector>

#define SUPPORTED_MIDI_NOTES 16

struct EspButtonMapping
{
    std::vector<uint8_t> midiNotesOn;
    std::vector<uint8_t> midiNotesOff;
    uint8_t columnIndex;
    uint8_t slotIndex;
    bool currentState = false;
};

class EspNowButtonController : public MidiController
{
private:
    ControlHub *hub;
    MidiDevice *midi;
    std::vector<EspButtonMapping> buttonMappings;
    bool noteStatus[SUPPORTED_MIDI_NOTES] = {false};

    const char *TAG = "ESP-NOW-BUTTON";

    const int MIDI_CHANNEL = 15;

public:
    EspNowButtonController(ControlHub *hub, MidiDevice *midi, std::vector<EspButtonMapping> buttonMappings)
    {
        this->hub = hub;
        this->midi = midi;
        this->buttonMappings = buttonMappings;

        hub->subscribe(this);
        midi->addMidiListener(this);

        // Log::info(TAG, "constructor");
    }

    ~EspNowButtonController()
    {
        hub->unsubscribe(this);
        midi->removeMidiListener(this);

        // Log::info(TAG, "destructor");
    }

    void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) override
    {
        noteStatus[note] = true; 
        Log::info(TAG, "ESP Note on received for note %d", note);
        updateSlots();
    }
    void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) override
    {
        noteStatus[note] = false; // Update note status
        Log::info(TAG, "ESP Note off received for note %d", note);
        updateSlots();
    }

    bool isMappingActive(EspButtonMapping map)
    {
        for (const auto &note : map.midiNotesOn)
            if (!noteStatus[note])
                return false;
        for (const auto &note : map.midiNotesOff)
            if (noteStatus[note])
                return false;
        return true;
    }

    void updateSlots()
    {
        for (int i = 0; i < buttonMappings.size(); i++)
        {
            EspButtonMapping &mapping = buttonMappings[i];
            bool shouldBeActive = isMappingActive(mapping);
            if (mapping.currentState == shouldBeActive)
                continue; 

            buttonMappings[i].currentState = shouldBeActive; 

            Log::info(TAG, "Slot %d %d is now %s", mapping.columnIndex, mapping.slotIndex, shouldBeActive ? "on" : "off");

            hub->setSlotActive(mapping.columnIndex, mapping.slotIndex, mapping.currentState);
        }
    }

    void onHubSlotActiveChange(int columnIndex, int slotIndex, uint8_t active) override {}

    void onControllerChange(uint8_t channel, uint8_t controller, uint8_t value) override {}
    void onSystemRealtime(uint8_t message) override {}
};
