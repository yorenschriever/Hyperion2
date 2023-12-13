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
        int TAP_NOTE_NUMBER;
        int TAP_STOP_NOTE_NUMBER;
        int TAP_ALIGN_NOTE_NUMBER;
        int MIDI_CHANNEL;
        int OFF;
        int GREEN;
    } Make;

    static Make MK1;
    static Make MK2;

private:
    ControlHub *hub;
    MidiDevice *midi;
    Make make;

    void handleKeyPress(int note, bool isOn)
    {
        if (note == make.TAP_NOTE_NUMBER && isOn)
            return TapTempo::getInstance()->Tap();
        if (note == make.TAP_STOP_NOTE_NUMBER && isOn)
            return TapTempo::getInstance()->Stop();
        if (note == make.TAP_ALIGN_NOTE_NUMBER && isOn)
            return TapTempo::getInstance()->Align();

        int column = note % make.WIDTH;
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
            hub->setMasterDim(scale127to255(value));
            return;
        }

        int column = controller - make.FADER0_CONTROLLER_NUMBER;
        if (column < 0 || column >= make.WIDTH)
            return;
        hub->dim(column, scale127to255(value));
    }
    void onSystemRealtime(uint8_t message) override {}

    void onHubSlotActiveChange(int columnIndex, int slotIndex, bool active) override
    {
        if (columnIndex < 0 || columnIndex > make.WIDTH - 1)
            return;
        if (slotIndex < 0 || slotIndex > make.HEIGHT - 1)
            return;
        // Log::info("APCMINI", "slot active change %d %d %d", columnIndex, slotIndex, active);
        int note = (make.HEIGHT - 1 - slotIndex) * make.WIDTH + columnIndex;
        if (note < 0 || note > make.WIDTH * make.HEIGHT)
            return;
        midi->sendNoteOn(make.MIDI_CHANNEL, note, active ? make.GREEN : make.OFF);
    }

    void setLeds()
    {
        midi->waitTxDone();
        Thread::sleep(10);
        for (int x = 0; x < make.WIDTH; x++)
        {
            for (int y = 0; y < make.HEIGHT; y++)
            {
                auto slot = hub->findSlot(x, y);
                bool isOn = slot && slot->activated;
                onHubSlotActiveChange(x, y, isOn);
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

    .TAP_NOTE_NUMBER = 98,
    .TAP_STOP_NOTE_NUMBER = 89,
    .TAP_ALIGN_NOTE_NUMBER = 88,

    .MIDI_CHANNEL = 0,

    .OFF = 0,
    .GREEN = 1};

ApcMiniController::Make ApcMiniController::MK2{
    .WIDTH = 8,
    .HEIGHT = 8,

    .FADER0_CONTROLLER_NUMBER = 48,
    .MASTER_DIM_FADER_CONTROLLER_NUMBER = 56,

    .TAP_NOTE_NUMBER = 0x76,
    .TAP_STOP_NOTE_NUMBER = 0x77,
    .TAP_ALIGN_NOTE_NUMBER = 0x7A,

    .MIDI_CHANNEL = 0,

    .OFF = 0,
    .GREEN = 21};