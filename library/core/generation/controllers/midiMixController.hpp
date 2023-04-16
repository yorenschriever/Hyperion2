#pragma once

#include "core/generation/controlHub/IHubController.hpp"
#include "core/generation/controlHub/controlHub.hpp"
#include "log.hpp"
#include "midiController.hpp"
#include "platform/includes/midi-device.hpp"
#include <algorithm>

class MidiMixController : public MidiController
{
private:
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
            return hub->setVelocity(scale127toFloat(value));
        if (controller == 20)
            return hub->setAmount(scale127toFloat(value));
        if (controller == 24)
            return hub->setSize(scale127toFloat(value));
        if (controller == 28)
            return hub->setOffset(scale127toFloat(value));
        if (controller == 46)
            return hub->setVariant(scale127toFloat(value));
        if (controller == 50)
            return hub->setIntensity(scale127toFloat(value));
    }
    //void onHubSlotActiveChange(int columnIndex, int slotIndex, bool active) override{}
    void onSystemRealtime(uint8_t message) override {}
    // void onHubColumnDimChange(int columnIndex, uint8_t dim) override {}
    // void onHubMasterDimChange(uint8_t dim) override {}
    // void onHubVelocityChange(float velocity) override {}
    // void onHubAmountChange(float amount) override {}
    // void onHubIntensityChange(float intensity) override {}
    // void onHubVariantChange(float variant) override {}
    // void onHubSizeChange(float size) override {}
    // void onHubOffsetChange(float offset) override {}
    void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) override {}
    void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) override {}
};
