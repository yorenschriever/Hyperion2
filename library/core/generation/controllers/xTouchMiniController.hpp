#pragma once

#include "core/generation/controlHub/IHubController.hpp"
#include "core/generation/controlHub/controlHub.hpp"
#include "log.hpp"
#include "midiController.hpp"
#include "midiDevice.hpp"
#include <algorithm>

class XTouchMiniController : public MidiController
{
private:
    ControlHub *hub;
    MidiDevice *midi;

    const char* TAG = "XTOUCHMINI";

    const int ROTARY1_CONTROLLER_NUMBER = 1;
    const int MIDI_CHANNEL = 10;

    const int paramsSlot = 0;
public:
    XTouchMiniController(ControlHub *hub, MidiDevice *midi)
    {
        this->hub = hub;
        this->midi = midi;

        hub->subscribe(this);
        midi->addMidiListener(this);

        hub->sendCurrentStatus(this);  
    }

    ~XTouchMiniController(){
        hub->unsubscribe(this);
        midi->removeMidiListener(this);

        //Log::info(TAG, "destructor");
    }

    void onControllerChange(uint8_t channel, uint8_t controller, uint8_t value) override
    {
        // Log::info(TAG, "onControllerChange %d %d %d", channel, controller, value);

        if (channel != MIDI_CHANNEL)
            return;

        if (controller == ROTARY1_CONTROLLER_NUMBER || controller == ROTARY1_CONTROLLER_NUMBER+10)
            return hub->setVelocity(paramsSlot,scale127toFloat(value));
        if (controller == ROTARY1_CONTROLLER_NUMBER+1 || controller == ROTARY1_CONTROLLER_NUMBER+11)
            return hub->setAmount(paramsSlot,scale127toFloat(value));
        if (controller == ROTARY1_CONTROLLER_NUMBER+2 || controller == ROTARY1_CONTROLLER_NUMBER+12)
            return hub->setSize(paramsSlot,scale127toFloat(value));
        if (controller == ROTARY1_CONTROLLER_NUMBER+3 || controller == ROTARY1_CONTROLLER_NUMBER+13)
            return hub->setOffset(paramsSlot,scale127toFloat(value));
        if (controller == ROTARY1_CONTROLLER_NUMBER+4 || controller == ROTARY1_CONTROLLER_NUMBER+14)
            return hub->setVariant(paramsSlot,scale127toFloat(value));
        if (controller == ROTARY1_CONTROLLER_NUMBER+5 || controller == ROTARY1_CONTROLLER_NUMBER+15)
            return hub->setIntensity(paramsSlot,scale127toFloat(value));
    }
    
    void onHubVelocityChange(int paramsSlotIndex, float velocity) override {
        if (paramsSlotIndex != paramsSlot) return;
        midi->sendControllerChange(MIDI_CHANNEL, ROTARY1_CONTROLLER_NUMBER, velocity*127);
        midi->sendControllerChange(MIDI_CHANNEL, ROTARY1_CONTROLLER_NUMBER+10, velocity*127);

    };
    void onHubAmountChange(int paramsSlotIndex, float amount) override {
        if (paramsSlotIndex != paramsSlot) return;
        midi->sendControllerChange(MIDI_CHANNEL, ROTARY1_CONTROLLER_NUMBER+1, amount*127);
        midi->sendControllerChange(MIDI_CHANNEL, ROTARY1_CONTROLLER_NUMBER+11, amount*127);
    };
    void onHubSizeChange(int paramsSlotIndex, float size) override {
        if (paramsSlotIndex != paramsSlot) return;
        midi->sendControllerChange(MIDI_CHANNEL, ROTARY1_CONTROLLER_NUMBER+2, size*127);
        midi->sendControllerChange(MIDI_CHANNEL, ROTARY1_CONTROLLER_NUMBER+12, size*127);
    };
    void onHubOffsetChange(int paramsSlotIndex, float offset) override {
        if (paramsSlotIndex != paramsSlot) return;
        midi->sendControllerChange(MIDI_CHANNEL, ROTARY1_CONTROLLER_NUMBER+3, offset*127);
        midi->sendControllerChange(MIDI_CHANNEL, ROTARY1_CONTROLLER_NUMBER+13, offset*127);
    };
    void onHubVariantChange(int paramsSlotIndex, float variant) override {
        if (paramsSlotIndex != paramsSlot) return;
        midi->sendControllerChange(MIDI_CHANNEL, ROTARY1_CONTROLLER_NUMBER+4, variant*127);
        midi->sendControllerChange(MIDI_CHANNEL, ROTARY1_CONTROLLER_NUMBER+14, variant*127);    
    };
    void onHubIntensityChange(int paramsSlotIndex, float intensity) override {
        if (paramsSlotIndex != paramsSlot) return;
        midi->sendControllerChange(MIDI_CHANNEL, ROTARY1_CONTROLLER_NUMBER+5, intensity*127);
        midi->sendControllerChange(MIDI_CHANNEL, ROTARY1_CONTROLLER_NUMBER+15, intensity*127);
    };

    void onSystemRealtime(uint8_t message) override {}
    void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) override {}
    void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) override {}
};
