#pragma once
#include "IHubController.hpp"
#include "colours.h"
#include "controlHub.hpp"
#include "gradient.hpp"

class PaletteColumn : public IHubController
{
public:
  struct Palette
  {
    Gradient *gradient;
    RGBA primary;
    RGBA secondary;
    RGBA highlight;
    std::string name;
  };

  PaletteColumn(ControlHub *hub, int columnIndex, int paramsSlotIndex, std::vector<Palette> palettes)
  {
    this->hub = hub;
    this->columnIndex = columnIndex;
    this->hub->expandTo(columnIndex, palettes.size()-1);
    this->palettes = palettes;
    this->paramsSlotIndex = paramsSlotIndex;

    for (int i = 0; i < palettes.size(); i++)
      hub->findSlot(columnIndex, i)->name = palettes[i].name;
  }

  void onHubSlotActiveChange(int columnIndex, int slotIndex, bool active) override
  {
    if (columnIndex != this->columnIndex)
      return;
    if (!active)
      return;
    if (slotIndex >= palettes.size())
      return;

    hub->getParams(paramsSlotIndex)->gradient = palettes[slotIndex].gradient;
    hub->getParams(paramsSlotIndex)->primaryColour = palettes[slotIndex].primary;
    hub->getParams(paramsSlotIndex)->secondaryColour = palettes[slotIndex].secondary;
    hub->getParams(paramsSlotIndex)->highlightColour = palettes[slotIndex].highlight;
  }

private:
  ControlHub *hub;
  int columnIndex = 0;
  int paramsSlotIndex=0;
  std::vector<Palette> palettes;
};
