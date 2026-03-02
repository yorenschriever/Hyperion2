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

  void onHubSlotActiveChange(int columnIndex, int slotIndex, uint8_t active) override
  {
    if (columnIndex != this->columnIndex)
      return;
    if (slotIndex >= palettes.size())
      return;

    if (!active)
    {
      if (active == ControlHub::DEFAULT)
        return;

      // a non default mode was turned off, fall back to a 'green' slot
      auto slots = hub->findColumn(columnIndex)->slots;
      int fallbackSlotsIndex = -1;
      for (int i = 0; i < slots.size(); i++)
      {
        if (slots[i].activated)
          fallbackSlotsIndex = i;
      }
      if (fallbackSlotsIndex != -1)
        slotIndex = fallbackSlotsIndex;
      else
        return;
    }

    for (auto paramsSlotIndex : paramsSlotIndices)
      hub->getParams(paramsSlotIndex)->palette = palettes[slotIndex];
  }

private:
  void init()
  {
    this->hub->expandTo(columnIndex, palettes.size() - 1);

    for (int i = 0; i < palettes.size(); i++)
      hub->findSlot(columnIndex, i)->name = palettes[i]->name;

    palettesCss.addPaletteColumn(*this);
  }

  ControlHub *hub;
  int columnIndex = 0;
  std::vector<int> paramsSlotIndices = {};
  std::vector<Palette *> palettes;

  class PalettesCss : public WebServerResponseBuilder
  {
    void build(WebServerResponseBuilder::Writer writer, void *userData) override
    {
      for (auto &output : paletteColumns)
      {

        write(userData, writer, ".column:nth-child(%d){\n", output.columnIndex + 1);
        write(userData, writer, " .slot::after {\n");
        write(userData, writer, "    content: \"\";\n");
        // write(userData, writer, "    border: 1px solid #eee;\n");
        write(userData, writer, "    position: absolute;\n");
        write(userData, writer, "    bottom: 8px;\n");
        write(userData, writer, "    left: 8px;\n");
        write(userData, writer, "    width: 64px;\n");
        write(userData, writer, "    height: 20px;\n");
        write(userData, writer, "    border-radius: 4px;\n");
        write(userData, writer, "  }\n");

        for (int slotIndex = 0; slotIndex < output.palettes.size(); slotIndex++)
        {
          auto &slot = output.palettes[slotIndex];
          write(userData, writer, "  .slot:nth-child(%d)::after {\n", slotIndex + 1);

          write(userData, writer, "    background: linear-gradient(to right");
          for (auto &color : slot->gradient.entries)
            write(userData, writer, ", rgba(%d, %d, %d, %f) %d%%", color.color.R, color.color.G, color.color.B, (float)color.color.A / 255.0, color.position*100/255);
          write(userData, writer, ") 0 100%% / 100%% 50%% no-repeat, ");
          
          write(userData, writer, "linear-gradient(to right, rgba(%d, %d, %d, %f) 0%% 33.33%%, rgba(%d, %d, %d, %f) 33.33%% 66.66%%, rgba(%d, %d, %d, %f) 66.66%% 100%%) 0 0 / 100%% 50%% no-repeat;\n",
                slot->primary.R, slot->primary.G, slot->primary.B, (float)slot->primary.A / 255.0,
                slot->secondary.R, slot->secondary.G, slot->secondary.B, (float)slot->secondary.A / 255.0,
                slot->highlight.R, slot->highlight.G, slot->highlight.B, (float)slot->highlight.A / 255.0);

          write(userData, writer, "  }\n");
        }

        write(userData, writer, "}\n\n");
      }
    }

    std::vector<PaletteColumn> paletteColumns;
    char buffer[500];

    void write(void *userData, WebServerResponseBuilder::Writer writer, const char *message, ...)
    {
      va_list args;
      va_start(args, message);
      int sz = vsnprintf(buffer, sizeof(buffer), message, args);
      writer(buffer, sz, userData);
      va_end(args);
    }

  public:
    void addPaletteColumn(PaletteColumn column)
    {
      paletteColumns.push_back(column);
    }
  };

  public:
  static PalettesCss palettesCss;
};

PaletteColumn::PalettesCss PaletteColumn::palettesCss;