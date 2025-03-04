#include "../common/distributeAndMonitor.hpp"
#include "../common/mapHelpers.hpp"
#include "core/hyperion.hpp"
#include "mapping/hexagons.hpp"
#include "mapping/schwungMap.hpp"
#include "patterns/testpatterns.hpp"

LUT *NeopixelLut = new ColourCorrectionLUT(1.5, 255, 255, 255, 255);

const int channelCount = 8;

// number of leds on each output
const int sizes[channelCount] = {100, 100, 100, 50, 100, 50, 100, 150};

void addPaletteColumn(Hyperion *hyp, const int col)
{
  auto paletteColumn = new PaletteColumn(
      &hyp->hub,
      col,
      0,
      {
          &orangeWhite,
          &campfire,
          &pinkSunset,
          &sunset8,
          &heatmap,
          &heatmap2,
          &sunset2,
          &retro,
          &tunnel,
          &plumBath,
          &matrix,

          &sunset6,
          &sunset7,
          &sunset1,
          &coralTeal,
          &deepBlueOcean,
          &redSalvation,
          &sunset4,
          &candy,
          &sunset3,
          &greatBarrierReef,
          &blueOrange,
          &peach,
          &denseWater,
          &purpleGreen,
          &sunset5,
          &salmonOnIce,
      });
  hyp->hub.subscribe(paletteColumn);
  hyp->hub.setColumnName(col, "Palette");
  hyp->hub.buttonPressed(col, 0);
  hyp->hub.setFlashColumn(col, false, true);
  hyp->hub.setForcedSelection(col);
}

int main()
{
  generateHexagons();

  auto hyp = new Hyperion();

  const int columnIndex = 1;

  PixelMap *map = &schwungMap;

  auto input = new ControlHubInput<RGBA>(
      map->size(),
      &hyp->hub,
      columnIndex,
      {
        new TestPatterns::OnPattern(RGB(255, 0, 0), "Red"),
        new TestPatterns::HexChaser(),
      });
  distributeAndMonitor<RGB, RGBA>(
      hyp, input, map,
      {
          {"hyperslave4.local", 9611, 100},
          {"hyperslave4.local", 9612, 100},
          {"hyperslave4.local", 9613, 100},
          {"hyperslave4.local", 9614, 50},
          {"hyperslave4.local", 9615, 100},
          {"hyperslave4.local", 9616, 50},
          {"hyperslave4.local", 9617, 100},
          {"hyperslave4.local", 9618, 150},
      });

  hyp->hub.setColumnName(columnIndex, "Led");
  hyp->hub.setFlashColumn(columnIndex, false, true);

  addPaletteColumn(hyp, 0);

  hyp->start();

  while (1)
    Thread::sleep(1000);
}