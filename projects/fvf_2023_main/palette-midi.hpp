#pragma once
#include "core/generation/controlHub/IHubController.hpp"
#include "core/generation/controlHub/controlHub.hpp"
#include "core/generation/controllers/midiController.hpp"
#include "core/hyperion.hpp"
#include "log.hpp"
#include "platform/includes/midiDevice.hpp"
#include <algorithm>

class DaslightLinkedController : public MidiController
{
private:
    ControlHub *hub;
    MidiDevice *midi;

    const char *TAG = "DaslightLinkedController";

    const int MIDI_CHANNEL = 5;
    const int HUB_COLUMN_INDEX = 0;

public:
    DaslightLinkedController(ControlHub *hub, MidiDevice *midi)
    {
        this->hub = hub;
        this->midi = midi;

        hub->subscribe(this);
        midi->addMidiListener(this);

        Log::info(TAG, "DaslightLinkedController constructor");
    }

    ~DaslightLinkedController()
    {
        hub->unsubscribe(this);
        midi->removeMidiListener(this);
    }

    void onHubSlotActiveChange(int columnIndex, int slotIndex, bool active) override
    {
        if (columnIndex != HUB_COLUMN_INDEX)
            return;
       // if (avoid_echo_note == slotIndex && avoid_echo_value == active)
        //    return;

        //avoid_echo_note = slotIndex;
        //avoid_echo_value = active;

        if (active)
        {
            midi->sendNoteOn(MIDI_CHANNEL, slotIndex, 127);
            midi->sendNoteOff(MIDI_CHANNEL, slotIndex, 0);
        }
        //else
        //    midi->sendNoteOn(MIDI_CHANNEL, slotIndex, 0);
    }

    void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) override
    {
        return; 
        if (channel != MIDI_CHANNEL)
            return;

        //if (avoid_echo_note == note && avoid_echo_value == velocity)
        //    return;

        //avoid_echo_note = note;
        //avoid_echo_value = velocity > 0;

        if (velocity==0) return;

        auto slot = hub->findSlot(HUB_COLUMN_INDEX,note);
        if (!slot) return;
        if (slot->activated) return;

        //if (velocity > 0)
            hub->buttonPressed(HUB_COLUMN_INDEX, note);
        //else
        //    hub->buttonReleased(HUB_COLUMN_INDEX, note);
    }

    void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) override
    {}
    //     return;
    //     onNoteOn(channel, note, 0);
    // }

    void onSystemRealtime(uint8_t message) override {}
    void onControllerChange(uint8_t channel, uint8_t controller, uint8_t value) override {}
};

class MidiControllerFactoryFvf : public MidiControllerFactory
{
public:
    std::unique_ptr<MidiController> create(MidiDevice *device, std::string name, ControlHub *hub) override
    {
        if (name.compare("MIDISPORT 2x2 Anniv B") == 0)
            return std::unique_ptr<DaslightLinkedController>(new DaslightLinkedController(hub, device));

        return MidiControllerFactory::create(device, name, hub);
    }
};