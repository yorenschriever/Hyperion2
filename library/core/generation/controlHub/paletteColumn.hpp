#pragma once
#include "IHubController.hpp"
#include "colors.h"
#include "controlHub.hpp"
#include "core/generation/gradient.hpp"
#include "core/generation/palette.hpp"

class PaletteColumn : public IHubController
{
public:

  PaletteColumn(ControlHub *hub, int columnIndex, std::vector<int> paramsSlotIndices, std::vector<Palette *> palettes)
  {
    this->hub = hub;
    this->columnIndex = columnIndex;
    this->paramsSlotIndices = paramsSlotIndices;
    this->palettes = palettes;
    init();
  }

  PaletteColumn(ControlHub *hub, int columnIndex, int paramsSlotIndex, std::vector<Palette *> palettes)
  {
    this->hub = hub;
    this->columnIndex = columnIndex;
    this->paramsSlotIndices = {paramsSlotIndex};
    this->palettes = palettes;
    init();
  }

  void onHubSlotActiveChange(int columnIndex, int slotIndex, bool active) override
  {
    if (columnIndex != this->columnIndex)
      return;
    if (!active)
      return;
    if (slotIndex >= palettes.size())
      return;

    for(auto paramsSlotIndex: paramsSlotIndices)
      hub->getParams(paramsSlotIndex)->palette = palettes[slotIndex];
  }

private:

  void init()
  {
    this->hub->expandTo(columnIndex, palettes.size() - 1);
    
    for (int i = 0; i < palettes.size(); i++)
      hub->findSlot(columnIndex, i)->name = palettes[i]->name;
  }

  ControlHub *hub;
  int columnIndex = 0;
  std::vector<int> paramsSlotIndices = {};
  std::vector<Palette *> palettes;
};
