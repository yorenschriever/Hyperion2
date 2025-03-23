#include "../common/distributeAndMonitor.hpp"
#include "../common/mapHelpers.hpp"
#include "../common/patterns/patterns-led.hpp"
#include "../common/patterns/patterns-mapped-2d.hpp"
#include "../common/patterns/patterns-mask.hpp"
#include "../common/patterns/patterns-test.hpp"
#include "core/hyperion.hpp"
#include "mapping/hexagons.hpp"
#include "mapping/schwungMap.hpp"
#include "patterns/testpatterns.hpp"
#include "patterns/patterns.hpp"
#include "patterns/masks.hpp"

LUT *NeopixelLut = new ColourCorrectionLUT(1.5, 255, 255, 255, 255);

void addPaletteColumn(Hyperion *hyp, const int col);
void addHexagonPipe(Hyperion * hyp);
void addlinePipe(Hyperion * hyp);

auto pHexagonMap = hexagonMap.toPolarRotate90();
auto pSchwungMap = schwungMap.toPolarRotate90();

void addPaletteColumn(Hyperion *hyp, const int col)
{
  auto paletteColumn = new PaletteColumn(
      &hyp->hub,
      col,
      0,
      {
          &campfire,
          &sunset8,
          &sunset2,
          &tunnel,
          &redSalvation,
          &denseWater,
          &plumBath,
          &matrix,
          
          &deepBlueOcean,
          &retro,
          &pinkSunset,
          &heatmap,
          &heatmap2,
          &sunset6,
          &sunset7,
          &sunset1,
          &coralTeal,
          &sunset4,
          &candy,
          &sunset3,
          &greatBarrierReef,
          &blueOrange,
          &peach,
          &purpleGreen,
          &sunset5,
          &salmonOnIce,
          &orangeWhite,
      });
  hyp->hub.subscribe(paletteColumn);
  hyp->hub.setColumnName(col, "Palette");
  hyp->hub.buttonPressed(col, 0);
  hyp->hub.setFlashColumn(col, false, true);
  hyp->hub.setForcedSelection(col);
}

void addHexagonPipe(Hyperion * hyp)
{
  generateHexagons();

  PixelMap *map = &schwungMap;
  PixelMap::Polar *pmap = &pSchwungMap;

  auto input = new ControlHubInput<RGBA>(
    
      map->size(),
      &hyp->hub,
      {

        //Static
        {.column = 1, .slot = 0, .pattern = new LedPatterns::PalettePattern(0, "Primary")},
        {.column = 1, .slot = 1, .pattern = new LedPatterns::PalettePattern(1, "Secondary")},
        {.column = 1, .slot = 2, .pattern = new SchwungPatterns::StaticGradientPattern(pmap)},
        {.column = 1, .slot = 3, .pattern = new LedPatterns::GlowPattern()},
        {.column = 1, .slot = 4, .pattern = new LedPatterns::RibbenClivePattern<Glow>(10000, 1, 0.025)},
        {.column = 1, .slot = 5, .pattern = new SchwungPatterns::OnBeatHexagonFade()},
        {.column = 1, .slot = 6, .pattern = new Mapped2dPatterns::HorizontalSaw(pmap)},
        {.column = 1, .slot = 7, .pattern = new SchwungPatterns::OnBeatHexagonFade()},

        //Map
        {.column = 2, .slot = 0, .pattern = new Mapped2dPatterns::Lighthouse(pmap)},
        {.column = 2, .slot = 1, .pattern = new Mapped2dPatterns::HorizontalSin(pmap)},
        {.column = 2, .slot = 2, .pattern = new Mapped2dPatterns::RadialSaw(pmap)},
        {.column = 2, .slot = 3, .pattern = new SchwungPatterns::AngularFadePattern(pmap)},
        {.column = 2, .slot = 4, .pattern = new SchwungPatterns::RadialFadePattern(pmap)},
        {.column = 2, .slot = 5, .pattern = new SchwungPatterns::RadialGlitterFadePattern(pmap)},
        {.column = 2, .slot = 6, .pattern = new SchwungPatterns::FadeFromCenter()},
        {.column = 2, .slot = 7, .pattern = new SchwungPatterns::FadeFromRandom()},

        //Bar
        {.column = 3, .slot = 0, .pattern = new LedPatterns::SegmentChasePattern()}, //alleen hexagons
        {.column = 3, .slot = 1, .pattern = new LedPatterns::GlowPulsePattern()},
        {.column = 3, .slot = 2, .pattern = new LedPatterns::RibbenFlashPattern()},  //alleen hexagons?
        {.column = 3, .slot = 3, .pattern = new Mapped2dPatterns::GrowingCirclesPattern(map)},
        {.column = 3, .slot = 4, .pattern = new Mapped2dPatterns::SpiralPattern(pmap)},
        {.column = 3, .slot = 5, .pattern = new Mapped2dPatterns::DotBeatPattern(pmap)}, //alleen hexagons, of grotere diameter
        {.column = 3, .slot = 6, .pattern = new SchwungPatterns::FadeStarPattern()}, //alleen hex
        {.column = 3, .slot = 7, .pattern = new SchwungPatterns::HexChaser()},

        //Strobe
        {.column = 4, .slot = 0, .pattern = new LedPatterns::FlashesPattern()},
        {.column = 4, .slot = 1, .pattern = new LedPatterns::StrobePattern()},
        {.column = 4, .slot = 2, .pattern = new LedPatterns::PixelGlitchPattern(10)},
        {.column = 4, .slot = 3, .pattern = new LedPatterns::SegmentGlitchPattern()},
        {.column = 4, .slot = 4, .pattern = new Mapped2dPatterns::LineLaunch(map)},
        {.column = 4, .slot = 5, .pattern = new SchwungPatterns::GrowingStrobePattern(pmap)},
        {.column = 4, .slot = 6, .pattern = new SchwungPatterns::HexagonStrobePattern()},

        //Mask
        {.column = 5, .slot = 0, .pattern = new SchwungMasks::OnBeatHexagonFade()},
        {.column = 5, .slot = 1, .pattern = new SchwungMasks::ParallelMaskPattern()},
        {.column = 5, .slot = 2, .pattern = new SchwungMasks::CentersMaskPattern()},
        {.column = 5, .slot = 3, .pattern = new SchwungMasks::EdgesMaskPattern()},
        {.column = 5, .slot = 4, .pattern = new SchwungMasks::StarsMaskPattern()},
        {.column = 5, .slot = 5, .pattern = new SchwungMasks::XY(map)},
        {.column = 5, .slot = 6, .pattern = new MaskPatterns::SinChaseMaskPattern()},
        {.column = 5, .slot = 7, .pattern = new MaskPatterns::GlowPulseMaskPattern()},
        
        //Flash 1
        {.column = 6, .slot = 0, .pattern = new LedPatterns::FlashesPattern()},
        {.column = 6, .slot = 1, .pattern = new LedPatterns::StrobePattern()},
        {.column = 6, .slot = 2, .pattern = new LedPatterns::PixelGlitchPattern(10)},
        {.column = 6, .slot = 3, .pattern = new LedPatterns::SegmentGlitchPattern()},
        {.column = 6, .slot = 4, .pattern = new Mapped2dPatterns::LineLaunch(map)},
        {.column = 6, .slot = 5, .pattern = new SchwungPatterns::GrowingStrobePattern(pmap)},
        {.column = 6, .slot = 6, .pattern = new SchwungPatterns::HexagonStrobePattern()},

        //Debug
        {.column = 8, .slot = 0, .pattern = new TestPatterns::ShowStarts(60)},
        {.column = 8, .slot = 1, .pattern = new TestPatterns::OneColor(RGB(255, 0, 0), "Red")},
        {.column = 8, .slot = 2, .pattern = new TestPatterns::OneColor(RGB(0, 255, 0), "Green")},
        {.column = 8, .slot = 3, .pattern = new TestPatterns::OneColor(RGB(0, 0, 255), "Blue")},
        {.column = 8, .slot = 4, .pattern = new TestPatterns::OneColor(RGB(255, 255, 255), "White")},
        {.column = 8, .slot = 5, .pattern = new TestPatterns::OneColor(RGB(127, 127, 127), "White 50%")},
        {.column = 8, .slot = 6, .pattern = new TestPatterns::Palette(10, 1)},
        {.column = 8, .slot = 7, .pattern = new TestPatterns::Gamma(10)}
      });
  distributeAndMonitor<RGB, RGBA>(
      hyp, input, map,
      {
          {"hypernode1.local", 9611, 8*60},
          {"hypernode1.local", 9612, 8*60},
          {"hypernode1.local", 9613, 8*60},
          {"hypernode1.local", 9614, 8*60},
          {"hypernode1.local", 9615, 8*60},
          {"hypernode1.local", 9616, 8*60},

          {"hypernode1.local", 9617, 8*60},
          {"hypernode1.local", 9618, 8*60},
      });

  hyp->hub.setColumnName(1, "Static");
  hyp->hub.setColumnName(2, "Map");
  hyp->hub.setColumnName(3, "Bar");
  hyp->hub.setColumnName(4, "Strobe");
  hyp->hub.setColumnName(5, "Mask");
  hyp->hub.setColumnName(6, "Flash");
  // hyp->hub.setColumnName(7, "Flash");
  hyp->hub.setColumnName(8, "Debug");
  
  hyp->hub.setFlashColumn(6);
  hyp->hub.setFlashColumn(7);
}

// void addLinePipe(Hyperion * hyp)
// {
//   const int columnIndex = 1;

//   PixelMap *map = &lineMap;

//   auto input = new ControlHubInput<RGBA>(
//       map->size(),
//       &hyp->hub,
//       columnIndex,
//       {
//         new TestPatterns::OnPattern(RGB(255, 0, 0), "Red"),
//         // new TestPatterns::HexChaser(),
//       });
//   distributeAndMonitor<RGB, RGBA>(
//       hyp, input, map,
//       {
//           {"hypernode1.local", 9617, 8*60},
//           {"hypernode1.local", 9618, 8*60},
//       });

//   hyp->hub.setColumnName(columnIndex, "Lines");
//   hyp->hub.setFlashColumn(columnIndex, false, true);
// }

int main()
{
  auto hyp = new Hyperion();

  

  addHexagonPipe(hyp);
  // addLinePipe(hyp);
  addPaletteColumn(hyp, 0);

  hyp->start();

  Tempo::AddSource(new ConstantTempo(120));

  while (1)
    Thread::sleep(1000);
}