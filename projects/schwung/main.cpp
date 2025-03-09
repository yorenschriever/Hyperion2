#include "../common/distributeAndMonitor.hpp"
#include "../common/mapHelpers.hpp"
#include "../common/patterns/patterns-led.hpp"
#include "../common/patterns/patterns-mapped-2d.hpp"
#include "core/hyperion.hpp"
#include "mapping/hexagons.hpp"
#include "mapping/schwungMap.hpp"
#include "patterns/testpatterns.hpp"
#include "patterns/patterns.hpp"

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

  const int columnIndex = 1;

  PixelMap *map = &hexagonMap;

  auto input = new ControlHubInput<RGBA>(
      map->size(),
      &hyp->hub,
      columnIndex,
      {
        new TestPatterns::OnPattern(RGB(255, 0, 0), "Red"),
        new TestPatterns::HexChaser(),

        new LedPatterns::GradientChasePattern(),
        new LedPatterns::GlowPulsePattern(),
        new LedPatterns::FlashesPattern(),
        new LedPatterns::StrobePattern(),
        new LedPatterns::PalettePattern(0, "Primary"),
        new LedPatterns::PalettePattern(1, "Secondary"),
        new LedPatterns::GlowPattern(),
        new LedPatterns::SegmentChasePattern(),
        new LedPatterns::PixelGlitchPattern(),
        new LedPatterns::FadingNoisePattern(),
        new LedPatterns::SinPattern(),
        new LedPatterns::GradientChasePattern(),
        new LedPatterns::GlowPulsePattern(),
        new LedPatterns::SegmentGlitchPattern(),

        new LedPatterns::RibbenClivePattern<Glow>(10000, 1, 0.025),
        new LedPatterns::RibbenFlashPattern(),  
        new Mapped2dPatterns::Lighthouse(&pHexagonMap),
        new Mapped2dPatterns::GrowingCirclesPattern(&hexagonMap),
        new Mapped2dPatterns::LineLaunch(&hexagonMap),
        new Mapped2dPatterns::SpiralPattern(&pHexagonMap),
        new Mapped2dPatterns::DotBeatPattern(&pHexagonMap),
        new Mapped2dPatterns::HorizontalSin(&pHexagonMap),
        new Mapped2dPatterns::HorizontalSaw(&pHexagonMap),
        new Mapped2dPatterns::RadialSaw(&pHexagonMap),
        new Mapped2dPatterns::GrowShrink(&pHexagonMap),
      

        new SchwungPatterns::AngularFadePattern(&pHexagonMap),
        new SchwungPatterns::GrowingStrobePattern(&pHexagonMap),
        new SchwungPatterns::RadialFadePattern(&pHexagonMap),
        new SchwungPatterns::StaticGradientPattern(&pHexagonMap),
        new SchwungPatterns::RadialGlitterFadePattern(&pHexagonMap),
        new SchwungPatterns::FadeFromCenter(),
        new SchwungPatterns::FadeFromRandom(),
        new SchwungPatterns::HexagonStrobePattern(),
        new SchwungPatterns::FadeStarPattern(),
        new SchwungPatterns::OnBeatHexagonFade(),
        new SchwungPatterns::HexChaser(),

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

  hyp->hub.setColumnName(columnIndex, "Hexagons");
  hyp->hub.setFlashColumn(columnIndex, false, true);
}

void addLinePipe(Hyperion * hyp)
{
  const int columnIndex = 2;

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

  while (1)
    Thread::sleep(1000);
}