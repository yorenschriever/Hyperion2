#include "colours.h"
#include "core/distribution/inputs/inputSplitter.hpp"
#include "core/distribution/inputs/patternInput.hpp"
#include "core/distribution/luts/colourCorrectionLut.hpp"
#include "core/distribution/outputs/cloneOutput.hpp"
#include "core/distribution/outputs/monitorOutput.hpp"
#include "core/distribution/outputs/neopixelOutput.hpp"
#include "core/distribution/outputs/udpOutput.hpp"
#include "core/distribution/pipes/convertPipe.hpp"
#include "core/generation/patterns/helpers/tempo/constantTempo.h"
#include "core/generation/pixelMap.hpp"
#include "core/generation/pixelMapSplitter.hpp"
#include "core/hyperion.hpp"
#include "distribution/inputs/controlHubInput.hpp"
#include "generation/controlHub/paletteColumn.hpp"
#include "mapping/wingMap.hpp"
#include "palettes.hpp"
#include "patterns.hpp"

#include "../ledster/mapping/ledsterMap.hpp"

void addWingsPipe(Hyperion *hyp);
void addPaletteColumn(Hyperion *hyp);

PixelMap::Polar pWingMap = wingMap.toPolarRotate90();

LUT *PixelLut = new ColourCorrectionLUT(1.5, 255, 255, 255, 240);

int main()
{
  auto hyp = new Hyperion();

  hyp->hub.params.primaryColour = RGB(255, 0, 255);
  hyp->hub.params.secondaryColour = RGB(50, 50, 50);
  hyp->hub.params.highlightColour = RGB(250, 250, 250);
  hyp->hub.params.gradient = &heatmap;

  addWingsPipe(hyp);
  addPaletteColumn(hyp);

  hyp->hub.setFlashColumn(7);

  Tempo::AddSource(new ConstantTempo(120));

  hyp->hub.buttonPressed(0, 0);

  hyp->start();

  while (1)
    Thread::sleep(1000);
}

void addWingsPipe(Hyperion *hyp)
{
  // Generate 1 pattern, and split it up in six outputs,
  // because UDPOutput (and therefore MonitorOutput) are limited by a
  // maximum transfer size of 2*1440 bytes
  auto splitInput = new InputSplitter(
      new ControlHubInput<RGBA>(
          wingMap.size(),
          &hyp->hub,
          {
              // static
              {.column = 1, .slot = 0, .pattern = new HorizontalGradientPattern(wingMap)},
              {.column = 1, .slot = 1, .pattern = new RadialGradientPattern(pWingMap)},

              // 
              {.column = 2, .slot = 0, .pattern = new ChevronsPattern(wingMap)},
              {.column = 2, .slot = 1, .pattern = new RadialGlitterFadePattern2(wingMap)},
              {.column = 2, .slot = 2, .pattern = new RadialRainbowPattern(pWingMap)},

              // 
              {.column = 3, .slot = 0, .pattern = new RadialFadePattern(wingMap)},
              {.column = 3, .slot = 1, .pattern = new GrowingCirclesPattern(wingMap)},
              {.column = 3, .slot = 2, .pattern = new AngularFadePattern(pWingMap)},

              // 
              {.column = 4, .slot = 0, .pattern = new RibbenClivePattern<Glow>(10000, 1, 0.15)},
              {.column = 4, .slot = 1, .pattern = new ClivePattern<SawDown>(32, 1000, 1, 0.1)},
              {.column = 4, .slot = 2, .pattern = new ClivePattern<SoftPWM>(32, 1000, 1, 0.5)},

              // 
              {.column = 5, .slot = 0, .pattern = new SegmentChasePattern()},
              {.column = 5, .slot = 1, .pattern = new Lighthouse(pWingMap)},
              {.column = 5, .slot = 2, .pattern = new XY(wingMap)},
              {.column = 5, .slot = 3, .pattern = new GlowPulsePattern()},

              // flash
              {.column = 6, .slot = 0, .pattern = new FlashesPattern()},
              {.column = 6, .slot = 1, .pattern = new StrobePattern()},
              {.column = 6, .slot = 2, .pattern = new GrowingStrobePattern(pWingMap)},
              {.column = 6, .slot = 3, .pattern = new LineLaunch(wingMap)},
              {.column = 6, .slot = 4, .pattern = new PixelGlitchPattern()},

              // flash
              {.column = 7, .slot = 0, .pattern = new FlashesPattern()},
              {.column = 7, .slot = 1, .pattern = new StrobePattern()},
              {.column = 7, .slot = 2, .pattern = new GrowingStrobePattern(pWingMap)},
              {.column = 7, .slot = 3, .pattern = new LineLaunch(wingMap)},
              {.column = 7, .slot = 4, .pattern = new PixelGlitchPattern()},
          }),
      {60 * 8 * sizeof(RGBA),
       60 * 8 * sizeof(RGBA),
       60 * 8 * sizeof(RGBA),
       60 * 8 * sizeof(RGBA)},
      true);

  auto splitMap = PixelMapSplitter(
      &wingMap, {60 * 8, 60 * 8, 60 * 8, 60 * 8});

  const char *hosts[4] = {
      "hyperslave1.local",
      "hyperslave1.local",
      "hyperslave2.local",
      "hyperslave2.local"};

  int ports[4] = {
      9611,
      9615,
      9611,
      9615};

  for (int i = 0; i < splitInput->size(); i++)
  {
    auto pipe = new ConvertPipe<RGBA, RGB>(
        splitInput->getInput(i),
        // new MonitorOutput(splitMap.getMap(i)));
        new CloneOutput({new MonitorOutput(splitMap.getMap(i)),
                         new UDPOutput(hosts[i], ports[i], 60)}));
    hyp->addPipe(pipe);
  }
}

void addPaletteColumn(Hyperion *hyp)
{
  auto paletteColumn = new PaletteColumn(&hyp->hub, 0, std::vector<PaletteColumn::Palette>{
    //coralTeal,
    plumBath,
    deepBlueOcean,
    heatmap2,
    salmonOnIce,
    // retro,
    // candy,
    greatBarrierReef,
    pinkSunset,
    //blueOrange,
    campfire,
    tunnel,
    {.gradient = &heatmap, .primary = heatmap.get(127), .secondary = heatmap.get(200), .highlight = heatmap.get(255)},
    {.gradient = &sunset1, .primary = sunset1.get(127), .secondary = sunset1.get(200), .highlight = sunset1.get(255)},
    purpleGreen,
    //{.gradient = &sunset3, .primary = sunset3.get(127), .secondary = sunset3.get(200), .highlight = sunset3.get(255)},
    //{.gradient = &sunset4, .primary = sunset4.get(127), .secondary = sunset4.get(200), .highlight = sunset4.get(255)},
    redSalvation,
    denseWater,
    });

  hyp->hub.subscribe(paletteColumn);
}


