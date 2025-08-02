
#pragma once

class ButtonMidiControllerFactory : public MidiControllerFactory
{
public:
  virtual std::unique_ptr<MidiController> create(MidiDevice *device, std::string name, ControlHub *hub) override
  {
    if (name.find("ESP-NOW button")==0)
        return std::unique_ptr<EspNowButtonController>(new EspNowButtonController(hub,device, 
          {
            {.midiNotesOn={1}, .columnIndex=9 , .slotIndex= 0},
            {.midiNotesOn={2}, .columnIndex=9 , .slotIndex= 1},
            {.midiNotesOn={3}, .columnIndex=9 , .slotIndex= 2},
            {.midiNotesOn={4}, .columnIndex=9 , .slotIndex= 3},
            {.midiNotesOn={5}, .columnIndex=9 , .slotIndex= 4},
            {.midiNotesOn={6}, .columnIndex=9 , .slotIndex= 5}
          }));
        
    // otherwise, return the value that the original MidiControllerFactory would have returned
    return MidiControllerFactory::create(device, name, hub);
  }
};