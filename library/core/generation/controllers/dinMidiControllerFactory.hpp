#pragma once
#include "midiControllerFactory.hpp"
#include "midiController.hpp"

template<class CONTROLLER>
class DinMidiControllerFactory : public MidiControllerFactory
{
public:
  // override the method that determines which controller to pick for a given device
  virtual std::unique_ptr<MidiController> create(MidiDevice *device, std::string name, ControlHub *hub) override
  {
    // if we are dealing with a din midi device, pick a specific controller. 
    // which one is defined through the template
    if (name.compare(UNKNOWN_DIN_MIDI_DEVICE) == 0)
      return std::unique_ptr<CONTROLLER>(new CONTROLLER(hub, device));

    // otherwise, return the value that the original MidiControllerFactory would have returned
    return MidiControllerFactory::create(device, name, hub);
  }
};