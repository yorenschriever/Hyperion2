#include "../common/distributeAndMonitor.hpp"
#include "../common/mapHelpers.hpp"
#include "../common/patterns/patterns-led.hpp"
#include "../common/patterns/patterns-mapped-2d.hpp"
#include "../common/patterns/patterns-mask.hpp"
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

void addHexagonPipe(Hyperion * hyp)
{
  generateHexagons();

  //const int columnIndex = 1;

  PixelMap *map = &hexagonMap;

  auto input = new ControlHubInput<RGBA>(
    
      map->size(),
      &hyp->hub,

      {

        //Static
        {.column = 2, .slot = 0, .pattern = new TestPatterns::OnPattern(RGB(255, 0, 0), "Red")},
        {.column = 2, .slot = 1, .pattern = new LedPatterns::PalettePattern(0, "Primary")},
        {.column = 2, .slot = 2, .pattern = new LedPatterns::PalettePattern(1, "Secondary")},
        {.column = 2, .slot = 3, .pattern = new SchwungPatterns::StaticGradientPattern(&pHexagonMap)},

        //Bar
        {.column = 3, .slot = 0, .pattern = new LedPatterns::GradientChasePattern()},
        {.column = 3, .slot = 1, .pattern = new LedPatterns::SegmentChasePattern()},
        {.column = 3, .slot = 2, .pattern = new LedPatterns::SinPattern()},
        {.column = 3, .slot = 3, .pattern = new Mapped2dPatterns::Lighthouse(&pHexagonMap)},
        {.column = 3, .slot = 4, .pattern = new Mapped2dPatterns::HorizontalSin(&pHexagonMap)},
        {.column = 3, .slot = 5, .pattern = new Mapped2dPatterns::HorizontalSaw(&pHexagonMap)},
        {.column = 3, .slot = 6, .pattern = new Mapped2dPatterns::RadialSaw(&pHexagonMap)},
        {.column = 3, .slot = 7, .pattern = new SchwungPatterns::AngularFadePattern(&pHexagonMap)},
        {.column = 3, .slot = 8, .pattern = new SchwungPatterns::RadialFadePattern(&pHexagonMap)},
        {.column = 3, .slot = 9, .pattern = new SchwungPatterns::RadialGlitterFadePattern(&pHexagonMap)},
        {.column = 3, .slot = 10, .pattern = new SchwungPatterns::HexChaser()},
        {.column = 3, .slot = 11, .pattern = new SchwungPatterns::FadeFromCenter()},
        {.column = 3, .slot = 12, .pattern = new SchwungPatterns::FadeFromRandom()},

        //Holistic
        {.column = 4, .slot = 0, .pattern = new LedPatterns::GlowPulsePattern()},
        {.column = 4, .slot = 1, .pattern = new LedPatterns::GlowPattern()},
        {.column = 4, .slot = 2, .pattern = new LedPatterns::RibbenClivePattern<Glow>(10000, 1, 0.025)},
        {.column = 4, .slot = 3, .pattern = new LedPatterns::RibbenFlashPattern()}, 
        {.column = 4, .slot = 4, .pattern = new Mapped2dPatterns::GrowingCirclesPattern(&hexagonMap)},
        {.column = 4, .slot = 5, .pattern = new Mapped2dPatterns::SpiralPattern(&pHexagonMap)},
        {.column = 4, .slot = 6, .pattern = new Mapped2dPatterns::DotBeatPattern(&pHexagonMap)},
        {.column = 4, .slot = 7, .pattern = new Mapped2dPatterns::GrowShrink(&pHexagonMap)},
        {.column = 4, .slot = 8, .pattern = new SchwungPatterns::OnBeatHexagonFade()},
        {.column = 4, .slot = 9, .pattern = new SchwungPatterns::FadeStarPattern()},

        //Strobe
        {.column = 5, .slot = 0, .pattern = new LedPatterns::FlashesPattern()},
        {.column = 5, .slot = 1, .pattern = new LedPatterns::StrobePattern()},
        {.column = 5, .slot = 2, .pattern = new LedPatterns::PixelGlitchPattern()},
        {.column = 5, .slot = 3, .pattern = new LedPatterns::SegmentGlitchPattern()},
        {.column = 5, .slot = 4, .pattern = new Mapped2dPatterns::LineLaunch(&hexagonMap)},
        {.column = 5, .slot = 5, .pattern = new SchwungPatterns::GrowingStrobePattern(&pHexagonMap)},
        {.column = 5, .slot = 6, .pattern = new SchwungPatterns::HexagonStrobePattern()},

        //Mask
        {.column = 6, .slot = 0, .pattern = new SchwungMasks::OnBeatHexagonFade()},
        {.column = 6, .slot = 1, .pattern = new SchwungMasks::ParallelMaskPattern()},
        {.column = 6, .slot = 2, .pattern = new SchwungMasks::CentersMaskPattern()},
        {.column = 6, .slot = 3, .pattern = new SchwungMasks::EdgesMaskPattern()},
        {.column = 6, .slot = 4, .pattern = new SchwungMasks::StarsMaskPattern()},
        {.column = 6, .slot = 5, .pattern = new SchwungMasks::XY(&hexagonMap)},
        {.column = 6, .slot = 6, .pattern = new MaskPatterns::SinChaseMaskPattern()},
        {.column = 6, .slot = 7, .pattern = new MaskPatterns::GlowPulseMaskPattern()},
        
        //Flash
        {.column = 7, .slot = 0, .pattern = new LedPatterns::FadingNoisePattern()},
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
      });

  hyp->hub.setColumnName(2, "Static");
  hyp->hub.setColumnName(3, "Bar");
  hyp->hub.setColumnName(4, "Holistic");
  hyp->hub.setColumnName(5, "Strobe");
  hyp->hub.setColumnName(6, "Mask");
  hyp->hub.setColumnName(7, "Flash");



  //hyp->hub.setFlashColumn(columnIndex, false, true);
}

void addLinePipe(Hyperion * hyp)
{
  const int columnIndex = 1;

  PixelMap *map = &lineMap;

  auto input = new ControlHubInput<RGBA>(
      map->size(),
      &hyp->hub,
      columnIndex,
      {
        new TestPatterns::OnPattern(RGB(255, 0, 0), "Red"),
        // new TestPatterns::HexChaser(),
      });
  distributeAndMonitor<RGB, RGBA>(
      hyp, input, map,
      {
          {"hypernode1.local", 9617, 8*60},
          {"hypernode1.local", 9618, 8*60},
      });

  hyp->hub.setColumnName(columnIndex, "Lines");
  hyp->hub.setFlashColumn(columnIndex, false, true);
}

int main()
{
  auto hyp = new Hyperion();

  

  addHexagonPipe(hyp);
  addLinePipe(hyp);
  addPaletteColumn(hyp, 0);

  hyp->start();

  Tempo::AddSource(new ConstantTempo(120));

  while (1)
    Thread::sleep(1000);
}