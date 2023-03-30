#pragma once
#include "controlHub.hpp"
#include "IHubController.hpp"
#include "core/generation/patterns/helpers/palette.hpp"

class PaletteColumn : public IHubController {
  public:
    PaletteColumn(ControlHub *hub, int columnIndex, std::vector<Palette*> palettes){
      this->hub = hub;
      this->columnIndex = columnIndex;
      this->hub->expandTo(columnIndex,palettes.size());
      this->palettes = palettes;
    }

    void onHubSlotActiveChange(int columnIndex, int slotIndex, bool active) override {
      if (columnIndex != this->columnIndex) 
        return;
      if (!active)
        return;
      if (slotIndex >= palettes.size()) 
        return;

      hub->params.palette = palettes[slotIndex];
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
  int columnIndex=0;
  std::vector<Palette*> palettes;
};
