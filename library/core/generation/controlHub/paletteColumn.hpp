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
    RGBA primary ;
    RGBA secondary;
    RGBA highlight;
  };

  PaletteColumn(ControlHub *hub, int columnIndex, std::vector<Palette> palettes)
  {
    this->hub = hub;
    this->columnIndex = columnIndex;
    this->hub->expandTo(columnIndex, palettes.size());
    this->palettes = palettes;
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

  void onHubColumnDimChange(int columnIndex, uint8_t dim) override {}
  void onHubMasterDimChange(uint8_t dim) override {}
  void onHubVelocityChange(float velocity) override {}
  void onHubAmountChange(float amount) override {}
  void onHubIntensityChange(float intensity) override {}
  void onHubVariantChange(float variant) override {}
  void onHubSizeChange(float size) override {}
  void onHubOffsetChange(float offset) override {}

private:
  ControlHub *hub;
  int columnIndex = 0;
  std::vector<Palette> palettes;
};
