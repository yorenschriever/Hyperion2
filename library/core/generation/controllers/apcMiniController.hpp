#pragma once

#include "core/generation/controlHub/IHubController.hpp"
#include "core/generation/controlHub/controlHub.hpp"
#include "log.hpp"
#include "midiController.hpp"
#include "platform/includes/midiDevice.hpp"
#include "thread.hpp"
#include "core/generation/patterns/helpers/tempo/tapTempo.h"

class ApcMiniController : public MidiController
{
public:
    typedef struct
    {
        int WIDTH;
        int HEIGHT;
        int FADER0_CONTROLLER_NUMBER;
        int MASTER_DIM_FADER_CONTROLLER_NUMBER;
        int PHRASE_ALIGN_NOTE_NUMBER;
        int TAP_NOTE_NUMBER;
        int TAP_STOP_NOTE_NUMBER;
        int TAP_ALIGN_NOTE_NUMBER;
        int TOGGLE_COLUMN_OFFSET_NOTE_NUMBER;
        int MIDI_CHANNEL;
        int OFF;
        int GREEN;
        int RED;
        int YELLOW;
    } Make;

    static Make MK1;
    static Make MK2;

private:
    ControlHub *hub;
    MidiDevice *midi;
    Make make;
    int columnOffset=0;

    void toggleColumnOffset()
    {
        bool hasOffset = columnOffset != 0;
        hasOffset = !hasOffset;

        Log::info("APCMINI", "Toggle not offset. on=%d", hasOffset);
        
        columnOffset = hasOffset ? 8:0;
        setLeds();
        midi->sendNoteOn(make.MIDI_CHANNEL, make.TOGGLE_COLUMN_OFFSET_NOTE_NUMBER, hasOffset ? make.GREEN : make.OFF);
    }

    void handleKeyPress(int note, bool isOn)
    {
        if (note == make.TAP_NOTE_NUMBER && isOn)
            return TapTempo::getInstance()->Tap();
        if (note == make.TAP_STOP_NOTE_NUMBER && isOn)
            return TapTempo::getInstance()->Stop();
        if (note == make.TAP_ALIGN_NOTE_NUMBER && isOn)
            return TapTempo::getInstance()->Align();
        if (note == make.PHRASE_ALIGN_NOTE_NUMBER && isOn)
            return Tempo::AlignPhrase();
        
        if (note == make.TOGGLE_COLUMN_OFFSET_NOTE_NUMBER && isOn)
            return toggleColumnOffset();

        int column = note % make.WIDTH + columnOffset;
        int row = make.HEIGHT - 1 - note / make.WIDTH;

        // Log::info("APCMINI", "keypress %d %d (%d %d)", note, isOn, column, row);
        if (isOn)
            hub->buttonPressed(column, row);
        else
            hub->buttonReleased(column, row);
    }

public:
    ApcMiniController(ControlHub *hub, MidiDevice *midi, Make make = MK1)
    {
        this->hub = hub;
        this->midi = midi;
        this->make = make;

        midi->addMidiListener(this);
        hub->subscribe(this, false);

        // Manually set all leds, instead of sendCurrentStatus,
        // because that also clears leds that have no slots in the control hub
        setLeds();

        // Log::info("APCMINI", "apcmini constructor");
    }

    ~ApcMiniController()
    {
        hub->unsubscribe(this);
        midi->removeMidiListener(this);

        // Log::info("APCMINI", "apcmini destructor");
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
        // Log::info("APCMINI", "onControllerChange %d %d", controller, value);

        if (controller == make.MASTER_DIM_FADER_CONTROLLER_NUMBER)
        {
            // Only the first device should control the master dim
            // to avoid flickering when 2 devices have their fader in a different
            // position and send an update every now and then
            if (columnOffset!=0) 
                return;

            hub->setMasterDim(scale127to255(value));
            return;
        }

        int column = controller - make.FADER0_CONTROLLER_NUMBER ;
        if (column < 0 || column >= make.WIDTH)
            return;
        column += columnOffset;
        hub->dim(column, scale127to255(value));
    }
    void onSystemRealtime(uint8_t message) override {}

    void onHubSlotActiveChange(int columnIndex, int slotIndex, uint8_t active) override
    {
        columnIndex -= columnOffset;
        if (columnIndex < 0 || columnIndex > make.WIDTH - 1)
            return;
        if (slotIndex < 0 || slotIndex > make.HEIGHT - 1)
            return;
        // Log::info("APCMINI", "slot active change %d %d %d", columnIndex, slotIndex, active);
        int note = (make.HEIGHT - 1 - slotIndex) * make.WIDTH + columnIndex;
        if (note < 0 || note > make.WIDTH * make.HEIGHT)
            return;

        int color = make.OFF;
        if (active & ControlHub::DEFAULT) color = make.GREEN;
        if (active & ControlHub::FLASH) color = make.RED;
        if (active & ControlHub::SEQUENCE) color = make.YELLOW;

        midi->sendNoteOn(make.MIDI_CHANNEL, note, color);
    }

    void setLeds()
    {
        midi->waitTxDone();
        Thread::sleep(10);
        for (int x = 0; x < make.WIDTH; x++)
        {
            for (int y = 0; y < make.HEIGHT; y++)
            {
                auto slot = hub->findSlot(x + columnOffset, y);
                bool isOn = slot && slot->activated;
                onHubSlotActiveChange(x + columnOffset, y, isOn);
            }
            midi->waitTxDone();
            Thread::sleep(5);
        }

        // clear the side leds
        for (int note = 82; note <= 89; note++)
            midi->sendNoteOn(make.MIDI_CHANNEL, note, make.OFF);
        midi->waitTxDone();
        Thread::sleep(5);

        // clear the bottom leds
        for (int note = 64; note <= 71; note++)
            midi->sendNoteOn(make.MIDI_CHANNEL, note, make.OFF);
        midi->waitTxDone();
        Thread::sleep(5);
    }
};

ApcMiniController::Make ApcMiniController::MK1 = {
    .WIDTH = 8,
    .HEIGHT = 8,

    .FADER0_CONTROLLER_NUMBER = 48,
    .MASTER_DIM_FADER_CONTROLLER_NUMBER = 56,

    .PHRASE_ALIGN_NOTE_NUMBER = 86,

    .TAP_NOTE_NUMBER = 98,
    .TAP_STOP_NOTE_NUMBER = 89,
    .TAP_ALIGN_NOTE_NUMBER = 88,

    .TOGGLE_COLUMN_OFFSET_NOTE_NUMBER = 82,

    .MIDI_CHANNEL = 0,

    .OFF = 0,
    .GREEN = 1,
    .RED = 3,
    .YELLOW = 5
};

ApcMiniController::Make ApcMiniController::MK2{
    .WIDTH = 8,
    .HEIGHT = 8,

    .FADER0_CONTROLLER_NUMBER = 48,
    .MASTER_DIM_FADER_CONTROLLER_NUMBER = 56,

    .PHRASE_ALIGN_NOTE_NUMBER = 74,

    .TAP_NOTE_NUMBER = 0x76,
    .TAP_STOP_NOTE_NUMBER = 0x77,
    .TAP_ALIGN_NOTE_NUMBER = 0x7A,

    .TOGGLE_COLUMN_OFFSET_NOTE_NUMBER = 0x70,

    .MIDI_CHANNEL = 0,

    .OFF = 0,
    .GREEN = 21,
    .RED = 5,
    .YELLOW = 9
};