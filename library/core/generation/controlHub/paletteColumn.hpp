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

  PaletteColumn(ControlHub *hub, int columnIndex, std::vector<Palette> palettes)
  {
    this->hub = hub;
    this->columnIndex = columnIndex;
    this->hub->expandTo(columnIndex, palettes.size()-1);
    this->palettes = palettes;

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

    hub->params.gradient = palettes[slotIndex].gradient;
    hub->params.primaryColour = palettes[slotIndex].primary;
    hub->params.secondaryColour = palettes[slotIndex].secondary;
    hub->params.highlightColour = palettes[slotIndex].highlight;
  }

private:
  ControlHub *hub;
  int columnIndex = 0;
  std::vector<Palette> palettes;
};
