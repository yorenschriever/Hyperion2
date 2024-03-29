#pragma once

#include <stdint.h>
#include <set>

#define UNKNOWN_DIN_MIDI_DEVICE "Unknown din midi device"

class IMidiListener;

class MidiDevice
{
public:
    using DestroyCallback = void (*)(void *userData);

    virtual void sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) = 0;
    virtual void sendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) = 0;
    virtual void sendControllerChange(uint8_t channel, uint8_t controller, uint8_t value) = 0;
    virtual bool waitTxDone(int timeout = 500) = 0;

    virtual bool isConnected() = 0;

    void addMidiListener(IMidiListener *listener) { listeners.insert(listener); }
    void removeMidiListener(IMidiListener *listener) { listeners.erase(listener); }

    virtual ~MidiDevice() = default;

protected:
    static const uint8_t MIDI_NOTE_ON = 0x90;
    static const uint8_t MIDI_NOTE_OFF = 0x80;
    static const uint8_t MIDI_CONTROLLER_CHANGE = 0xB0;
    static const uint8_t MIDI_NUMBER_OF_CONTROLLERS = 120;
    static const uint8_t MIDI_NUMBER_OF_NOTES = 127;

    std::set<IMidiListener *>listeners;
};
