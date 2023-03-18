#pragma once

#include "log.hpp"
#include "midi-listener.hpp"
#include "platform/includes/midi-device.hpp"
#include "rtmidi/RtMidi.h"
#include <map>
#include <stdint.h>
#include <vector>

class MidiDeviceMacos : public MidiDevice
{
public:
  MidiDeviceMacos(int in_index, int out_index)
  {
    if (out_index >= 0)
      midiOut.openPort(out_index);

    if (in_index >= 0)
    {
      midiIn.setCallback(&callback, this);
      midiIn.openPort(in_index);
      // ignore sysex and active sensing messages.
      midiIn.ignoreTypes(true, false, true);
    }
  }

  void sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) override
  {
    const unsigned char data[3] = {(unsigned char)(MIDI_NOTE_ON | channel), note, velocity};
    midiOut.sendMessage(data, sizeof(data));
  }

  void sendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) override
  {
    const unsigned char data[3] = {(unsigned char)(MIDI_NOTE_OFF | channel), note, velocity};
    midiOut.sendMessage(data, sizeof(data));
  }

  void sendControllerChange(uint8_t channel, uint8_t controller, uint8_t value) override
  {
    const unsigned char data[3] = {(unsigned char)(MIDI_CONTROLLER_CHANGE | channel), controller, value};
    midiOut.sendMessage(data, sizeof(data));
  }

  bool waitTxDone(int timeout = 500) override
  {
    return true; // this is not exposed on rtmidi :s
  };

  bool isConnected() override
  {
    return midiIn.isPortOpen() && midiOut.isPortOpen();
  }

private:
  RtMidiIn midiIn;
  RtMidiOut midiOut;

  static void callback(double delta_time, std::vector<unsigned char> *message, void *userData)
  {
    auto instance = (MidiDeviceMacos *)userData;
    auto listener = instance->listener;

    if (!listener)
      return;

    if (message->size() == 1 && message->at(0) > 0xF8)
    {
      listener->onSystemRealtime(message->at(0));
      return;
    }

    if (message->size() == 3)
    {

      uint8_t channel = message->at(0) & 0x0F;
      uint8_t message_type = message->at(0) & 0xF0;

      if (message_type == MIDI_NOTE_ON && message->at(1) < MIDI_NUMBER_OF_NOTES)
      {
        listener->onNoteOn(channel, message->at(1), message->at(2));
        return;
      }

      if (message_type == MIDI_NOTE_OFF && message->at(1) < MIDI_NUMBER_OF_NOTES)
      {
        listener->onNoteOff(channel, message->at(1), message->at(2));
        return;
      }

      if (message_type == MIDI_CONTROLLER_CHANGE && message->at(1) < MIDI_NUMBER_OF_CONTROLLERS)
      {
        listener->onControllerChange(channel, message->at(1), message->at(2));
        return;
      }
    }

    Log::error("MIDI", "Unknown message");
  }
};
