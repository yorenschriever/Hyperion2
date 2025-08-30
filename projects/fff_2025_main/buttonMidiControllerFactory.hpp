
#pragma once

class ButtonMidiControllerFactory : public MidiControllerFactory
{
public:
  virtual std::unique_ptr<MidiController> create(MidiDevice *device, std::string name, ControlHub *hub) override
  {
    if (name.find("ESP-NOW button")==0)
        return std::unique_ptr<EspNowButtonController>(new EspNowButtonController(hub,device, 
          {
            {.midiNotesOn={1}, .columnIndex=16 , .slotIndex= 0},
            {.midiNotesOn={2}, .columnIndex=16 , .slotIndex= 1},
            {.midiNotesOn={3}, .columnIndex=16 , .slotIndex= 2},
            {.midiNotesOn={4}, .columnIndex=16 , .slotIndex= 3},
            {.midiNotesOn={5}, .columnIndex=16 , .slotIndex= 4},
            {.midiNotesOn={6}, .columnIndex=16 , .slotIndex= 5},

            {.midiNotesOn={7}, .columnIndex=15 , .slotIndex= 6}, //rood, fire
            {.midiNotesOn={8}, .columnIndex=6 , .slotIndex= 3}, // groen, strobe
            {.midiNotesOn={9}, .columnIndex=6 , .slotIndex= 7}, //paars, smoke
            {.midiNotesOn={10}, .columnIndex=13 , .slotIndex= 4}, //Wit, glitch
            {.midiNotesOn={10}, .columnIndex=6 , .slotIndex= 7}, // wit, glitch
            {.midiNotesOn={11}, .columnIndex=15 , .slotIndex= 5}, // Geel, fire
          }));
        
    // otherwise, return the value that the original MidiControllerFactory would have returned
    return MidiControllerFactory::create(device, name, hub);
  }
};