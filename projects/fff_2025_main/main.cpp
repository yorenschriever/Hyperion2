#include "../common/distributeAndMonitor.hpp"
#include "../common/patterns/patterns-led.hpp"
#include "../common/patterns/patterns-test.hpp"
#include "../common/setViewParams.hpp"
#include "../common/paletteColumn.hpp"
#include "core/hyperion.hpp"
#include "mapping/cageMap.hpp"

#include "buttonMidiControllerFactory.hpp"

//TODO write own pattern for this project
#include "patterns-flash.hpp"
#include "patterns-hi.hpp"
#include "patterns-low.hpp"
#include "patterns-max.hpp"
#include "patterns-mid.hpp"
#include "patterns-min.hpp"

#include "patterns-button.hpp"

LUT *ledBarLut = new ColourCorrectionLUT(1.8, 255, 200, 200, 200);

PixelMap3d::Cylindrical cCageMap = cageMap.toCylindricalXZ();
PixelMap3d::Spherical sCageMap = cageMap.toSphericalXZ();

void addCagePipe(Hyperion *hyp)
{

  PixelMap3d *map = &cageMap;
  PixelMap3d::Cylindrical *cmap = &cCageMap;
  PixelMap3d::Spherical *smap = &sCageMap;
  
  auto input = new ControlHubInput<RGBA>(

      map->size(),
      &hyp->hub,
      {
            {.column = 1, .slot = 0, .pattern = new Max::ChevronsPattern(map)},
            {.column = 1, .slot = 1, .pattern = new Max::ChevronsConePattern(cmap)},
            {.column = 1, .slot = 2, .pattern = new Max::RadialFadePattern(cmap)},
            {.column = 1, .slot = 3, .pattern = new Max::RadialGlitterFadePattern(cmap)},
            {.column = 1, .slot = 4, .pattern = new Max::AngularFadePattern(cmap)},
            {.column = 1, .slot = 5, .pattern = new Max::GrowingStrobePattern(cmap)},

            {.column = 2, .slot = 0, .pattern = new Min::RibbenClivePattern<Glow>(10000, 1, 0.15)},
            {.column = 2, .slot = 1, .pattern = new Min::RibbenFlashPattern()},
            {.column = 2, .slot = 2, .pattern = new Min::GrowingCirclesPattern(map)},
            {.column = 2, .slot = 3, .pattern = new Min::SpiralPattern(cmap)},
            {.column = 2, .slot = 4, .pattern = new Min::SegmentChasePattern()},
            {.column = 2, .slot = 5, .pattern = new Min::GlowPulsePattern()},
            {.column = 2, .slot = 6, .pattern = new Min::LineLaunch(map)},

            {.column = 3, .slot = 0, .pattern = new Low::HorizontalSin(cmap)},
            {.column = 3, .slot = 1, .pattern = new Low::VerticallyIsolated(cmap)},
            {.column = 3, .slot = 2, .pattern = new Low::HorizontalSaw(cmap)},
            {.column = 3, .slot = 3, .pattern = new Low::StaticGradientPattern(map)},
            {.column = 3, .slot = 4, .pattern = new Low::OnBeatColumnChaseUpPattern(map)},
            {.column = 3, .slot = 5, .pattern = new Low::GrowShrink(cmap)},
            {.column = 3, .slot = 6, .pattern = new Low::RotatingRingsPattern(cmap)},
            {.column = 3, .slot = 7, .pattern = new Low::GlowPulsePattern(map)},

            // {.column = 4, .slot = 0, .pattern = new Mid::HaloOnBeat(cmap)},
            {.column = 4, .slot = 1, .pattern = new Mid::Halo(cmap)},
            {.column = 4, .slot = 2, .pattern = new Mid::SnowflakePatternColumn(cmap)},
            {.column = 4, .slot = 3, .pattern = new Mid::TakkenChase(cmap)},
            {.column = 4, .slot = 5, .pattern = new Mid::Lighthouse(cmap)},
            {.column = 4, .slot = 6, .pattern = new Mid::FireworksPattern(map)},
            {.column = 4, .slot = 7, .pattern = new Mid::DoubleFlash(cmap)},

            {.column = 5, .slot = 0, .pattern = new Hi::DotBeatPattern(cmap)},
            {.column = 5, .slot = 5, .pattern = new Hi::XY(map)},

            {.column = 7, .slot = 0, .pattern = new Flash::FlashesPattern()},
            {.column = 7, .slot = 1, .pattern = new Flash::SquareGlitchPattern(map)},
            {.column = 7, .slot = 3, .pattern = new Flash::PixelGlitchPattern()},
            {.column = 7, .slot = 4, .pattern = new Max::GrowingStrobePattern(cmap)},
            {.column = 7, .slot = 5, .pattern = new Flash::StrobeHighlightPattern()},
            {.column = 7, .slot = 6, .pattern = new Flash::StrobePattern()},
            {.column = 7, .slot = 7, .pattern = new Flash::FadingNoisePattern()},

            // {.column = 8, .slot = 0, .pattern = new TestPatterns::ShowStarts(60)},
            // {.column = 8, .slot = 1, .pattern = new TestPatterns::OneColor(RGB(255, 0, 0), "Red")},
            // {.column = 8, .slot = 2, .pattern = new TestPatterns::OneColor(RGB(0, 255, 0), "Green")},
            // {.column = 8, .slot = 3, .pattern = new TestPatterns::OneColor(RGB(0, 0, 255), "Blue")},
            // {.column = 8, .slot = 4, .pattern = new TestPatterns::OneColor(RGB(255, 255, 255), "White")},
            // {.column = 8, .slot = 5, .pattern = new TestPatterns::OneColor(RGB(127, 127, 127), "White 50%")},
            // {.column = 8, .slot = 6, .pattern = new TestPatterns::Palette(120, 20)},
            // {.column = 8, .slot = 7, .pattern = new TestPatterns::Gamma(60)},
            // {.column = 8, .slot = 8, .pattern = new TestPatterns::BrightnessMatch()},


          // // Static
          // {.column = 1, .slot = 0, .pattern = new LedPatterns::PalettePattern(0, "Primary")},
          // {.column = 1, .slot = 1, .pattern = new LedPatterns::PalettePattern(1, "Secondary")},

          {.column = 8, .slot = 0, .pattern = new TestPatterns::ShowStarts(60, 11)},
          {.column = 8, .slot = 1, .pattern = new TestPatterns::OneColor(RGB(255, 0, 0), "Red")},
          {.column = 8, .slot = 2, .pattern = new TestPatterns::OneColor(RGB(0, 255, 0), "Green")},
          {.column = 8, .slot = 3, .pattern = new TestPatterns::OneColor(RGB(0, 0, 255), "Blue")},
          {.column = 8, .slot = 4, .pattern = new TestPatterns::OneColor(RGB(255, 255, 255), "White")},
          {.column = 8, .slot = 5, .pattern = new TestPatterns::OneColor(RGB(127, 127, 127), "White 50%")},
          {.column = 8, .slot = 6, .pattern = new TestPatterns::Palette(10, 1)},
          {.column = 8, .slot = 7, .pattern = new TestPatterns::Gamma(10)},

          {.column = 9, .slot = 0, .pattern = new Buttons::ButtonPressedPattern(map,0)},
          {.column = 9, .slot = 1, .pattern = new Buttons::ButtonPressedPattern(map,1)},
          {.column = 9, .slot = 2, .pattern = new Buttons::ButtonPressedPattern(map,2)},
          {.column = 9, .slot = 3, .pattern = new Buttons::ButtonPressedPattern(map,3)},
          {.column = 9, .slot = 4, .pattern = new Buttons::ButtonPressedPattern(map,4)},
          {.column = 9, .slot = 5, .pattern = new Buttons::ButtonPressedPattern(map,5)},
          {.column = 9, .slot = 6, .pattern = new Buttons::SyncToMeasurePattern(smap)},

      });

  distributeAndMonitor3d<BGR, RGBA>(
      hyp, input, map,
      {
          {"hypernode1.local", 9611, 6 * 60},
          {"hypernode1.local", 9612, 6 * 60},
          {"hypernode1.local", 9613, 6 * 60},
          {"hypernode1.local", 9614, 6 * 60},
          {"hypernode1.local", 9615, 6 * 60},
          {"hypernode1.local", 9616, 6 * 60},
          {"hypernode1.local", 9617, 6 * 60},
          {"hypernode1.local", 9618, 6 * 60},
      }, ledBarLut);

  hyp->hub.setColumnName(1, "Static");
  hyp->hub.buttonPressed(9,0);
}

int main()
{
  auto hyp = new Hyperion();
  hyp->setMidiControllerFactory(new ButtonMidiControllerFactory());

  Tempo::AddSource(new ConstantTempo(120));

  setupPaletteColumn(hyp);
  addCagePipe(hyp);

  hyp->start();
  // setViewParams(hyp, viewParamsDefault);
  // setViewParams(hyp, viewParamsTop);
  // setViewParams(hyp, viewParamsFront);
  // setViewParams(hyp, viewParamsSide);
  auto viewParams = new ViewParams(
      40, 
      -0.45, 
      Vector{0, 0.1, -2.5}, 
      Rotation{.4, 1, 0, 0},
      Rotation{0, 0, 1, 0});
  setViewParams(hyp, viewParams);

  while (1)
  {
    Thread::sleep(1000);
  }
}
