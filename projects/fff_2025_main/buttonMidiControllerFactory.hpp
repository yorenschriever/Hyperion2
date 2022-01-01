
#pragma once
#include "columns.hpp"
class ButtonMidiControllerFactory : public MidiControllerFactory
{
public:
  virtual std::unique_ptr<MidiController> create(MidiDevice *device, std::string name, ControlHub *hub) override
  {
    if (name.find("ESP-NOW button")==0)
        return std::unique_ptr<EspNowButtonController>(new EspNowButtonController(hub,device, 
          {
            {.midiNotesOn={1}, .columnIndex=Columns::BUTTONS , .slotIndex= 0},
            {.midiNotesOn={2}, .columnIndex=Columns::BUTTONS , .slotIndex= 1},
            {.midiNotesOn={3}, .columnIndex=Columns::BUTTONS , .slotIndex= 2},
            {.midiNotesOn={4}, .columnIndex=Columns::BUTTONS , .slotIndex= 3},
            {.midiNotesOn={5}, .columnIndex=Columns::BUTTONS , .slotIndex= 4},
            {.midiNotesOn={6}, .columnIndex=Columns::BUTTONS , .slotIndex= 5},

            {.midiNotesOn={7},  .columnIndex=Columns::EFFECTS ,     .slotIndex= 6}, // rood, fire
            {.midiNotesOn={8},  .columnIndex=Columns::CAGE_FLASH ,  .slotIndex= 3}, // groen, strobe
            {.midiNotesOn={9},  .columnIndex=Columns::FOG ,         .slotIndex= 0}, // paars, smoke
            {.midiNotesOn={10}, .columnIndex=Columns::WINGS_FLASH , .slotIndex= 4}, // Wit, glitch
            {.midiNotesOn={10}, .columnIndex=Columns::CAGE_FLASH ,  .slotIndex= 7}, // wit, glitch
            {.midiNotesOn={11}, .columnIndex=Columns::EFFECTS ,     .slotIndex= 5}, // Geel, fire
          }));
        
    // otherwise, return the value that the original MidiControllerFactory would have returned
    return MidiControllerFactory::create(device, name, hub);
  }
};