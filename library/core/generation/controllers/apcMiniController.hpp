#pragma once

#include "core/generation/controlHub/IHubController.hpp"
#include "core/generation/controlHub/controlHub.hpp"
#include "log.hpp"
#include "midiController.hpp"
#include "platform/includes/midi-device.hpp"

class ApcMiniController : public MidiController
{
private:
    ControlHub *hub;
    MidiDevice *midi;

    const int WIDTH = 8;
    const int HEIGHT = 8;

    const int FADER0_CONTROLLER_NUMBER = 48; 
    const int MASTER_DIM_FADER_CONTROLLER_NUMBER = 56;

    const int MIDI_CHANNEL = 0;

    const int OFF = 0;
    const int GREEN = 1;
    const int GREEN_BLINK = 2;
    const int RED = 3;
    const int RED_BLINK = 4;
    const int ORANGE = 5;
    const int ORANGE_BLINK = 6;

    void handleKeyPress(int note, bool isOn)
    {
        int column = note % WIDTH;
        int row = HEIGHT - 1 - note / WIDTH;

        //Log::info("APCMINI", "keypress %d %d (%d %d)", note, isOn, column, row);

        if (isOn)
            hub->buttonPressed(column, row);
        else
            hub->buttonReleased(column, row);
    }

public:
    ApcMiniController(ControlHub *hub, MidiDevice *midi)
    {
        this->hub = hub;
        this->midi = midi;

        hub->subscribe(this);
        midi->addMidiListener(this);

        //Log::info("APCMINI", "apcmini constructor");
    }

    ~ApcMiniController(){
        hub->unsubscribe(this);
        midi->removeMidiListener(this);

        //Log::info("APCMINI", "apcmini destructor");
    }

    void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) override
    {
        this->handleKeyPress(note, velocity > 0);
    }
    void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) override
    {
        this->handleKeyPress(note, false);
    }

    void onControllerChange(uint8_t channel, uint8_t controller, uint8_t value) override
    {
        //Log::info("APCMINI", "onControllerChange %d %d", controller, value);

        if (controller == MASTER_DIM_FADER_CONTROLLER_NUMBER)
        {
            hub->setMasterDim(scale127to255(value));
            return;
        }

        int column = controller - FADER0_CONTROLLER_NUMBER;
        if (column < 0 || column >= WIDTH)
            return;
        hub->dim(column, scale127to255(value));
    }
    void onSystemRealtime(uint8_t message) override {}

    void onHubSlotActiveChange(int columnIndex, int slotIndex, bool active) override
    {
        //Log::info("APCMINI", "slot active change %d %d %d", columnIndex, slotIndex, active);
        int note = (HEIGHT - 1 - slotIndex) * WIDTH + columnIndex;
        if (note < 0 || note > WIDTH * HEIGHT)
            return;
        midi->sendNoteOn(MIDI_CHANNEL, note, active ? GREEN : OFF);
    }
    void onHubColumnDimChange(int columnIndex, uint8_t dim) override {}
    void onHubMasterDimChange(uint8_t dim) override {}
};
