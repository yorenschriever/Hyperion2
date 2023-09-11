#pragma once

#include "core/generation/controlHub/IHubController.hpp"
#include "core/generation/controlHub/controlHub.hpp"
#include "log.hpp"
#include "midiController.hpp"
#include "midiDevice.hpp"
#include <algorithm>

class KorgNanoKontrol2controller : public MidiController
{
private:
    ControlHub *hub;
    MidiDevice *midi;

    const char* TAG = "NANOKONTROL";

    const int WIDTH = 8;
    const int HEIGHT = 3;

    const int FADER0_CONTROLLER_NUMBER = 0; 
    const int MASTER_DIM_FADER_CONTROLLER_NUMBER = 23;
    const int ROTARY0_CONTROLLER_NUMBER = 16;

    const std::vector<uint8_t> BUTTON_MAPPING = {
        32,33,34,35,36,37,38,39, 
        48,49,50,51,52,53,54,55, 
        64,65,66,67,68,69,70,71}
    ;

    const int MIDI_CHANNEL = 0;

    const int OFF = 0;
    const int RED = 127;

public:
    KorgNanoKontrol2controller(ControlHub *hub, MidiDevice *midi)
    {
        this->hub = hub;
        this->midi = midi;

        hub->subscribe(this);
        midi->addMidiListener(this);

        //Log::info(TAG, "constructor");
    }

    ~KorgNanoKontrol2controller(){
        hub->unsubscribe(this);
        midi->removeMidiListener(this);

        //Log::info(TAG, "destructor");
    }

    void onControllerChange(uint8_t channel, uint8_t controller, uint8_t value) override
    {
        //Log::info(TAG, "onControllerChange %d %d", controller, value);

        int paramsSlot = 0;

        if (controller == ROTARY0_CONTROLLER_NUMBER)
            return hub->setVelocity(paramsSlot,scale127toFloat(value));
        if (controller == ROTARY0_CONTROLLER_NUMBER+1)
            return hub->setIntensity(paramsSlot,scale127toFloat(value));
        if (controller == ROTARY0_CONTROLLER_NUMBER+2)
            return hub->setVariant(paramsSlot,scale127toFloat(value));
        if (controller == ROTARY0_CONTROLLER_NUMBER+3)
            return hub->setSize(paramsSlot,scale127toFloat(value));
        if (controller == ROTARY0_CONTROLLER_NUMBER+4)
            return hub->setOffset(paramsSlot,scale127toFloat(value));
        if (controller == ROTARY0_CONTROLLER_NUMBER+5)
            return hub->setAmount(paramsSlot,scale127toFloat(value));
        //if (controller == ROTARY0_CONTROLLER_NUMBER+6)
        //    todo palette
        if (controller == MASTER_DIM_FADER_CONTROLLER_NUMBER)
            return hub->setMasterDim(scale127to255(value));

        int column = controller - FADER0_CONTROLLER_NUMBER;
        if (column >= 0 && column < WIDTH)
            return hub->dim(column, scale127to255(value));

        auto found = std::find(BUTTON_MAPPING.begin(), BUTTON_MAPPING.end(), controller);
        if (found != BUTTON_MAPPING.end()){
            int index = std::distance(BUTTON_MAPPING.begin(), found);
            int column = index % WIDTH;
            int row = index / WIDTH;

            if (value > 0)
                hub->buttonPressed(column, row);
            else
                hub->buttonReleased(column, row);
        }
    }
    
    void onHubSlotActiveChange(int columnIndex, int slotIndex, bool active) override
    {
        int controller = slotIndex * WIDTH + columnIndex;
        if (controller < 0 || controller > WIDTH * HEIGHT)
            return;
        midi->sendControllerChange(MIDI_CHANNEL, BUTTON_MAPPING[controller], active ? RED : OFF);
    }

    void onSystemRealtime(uint8_t message) override {}
    void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) override {}
    void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) override {}
};
