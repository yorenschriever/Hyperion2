#include "colours.h"
#include "core/distribution/inputs/inputSplitter.hpp"
#include "core/distribution/inputs/patternInput.hpp"
#include "core/distribution/luts/colourCorrectionLut.hpp"
#include "core/distribution/outputs/monitorOutput.hpp"
#include "core/distribution/outputs/neopixelOutput.hpp"
#include "core/distribution/pipes/convertPipe.hpp"
#include "core/generation/patterns/helpers/tempo/constantTempo.h"
#include "core/generation/pixelMap.hpp"
#include "core/generation/pixelMapSplitter.hpp"
#include "core/hyperion.hpp"
#include "distribution/inputs/controlHubInput.hpp"
#include "mapping/wingMap.hpp"
#include "patterns.hpp"
#include "palettes.hpp"
#include "generation/controlHub/paletteColumn.hpp"
#include "core/distribution/outputs/cloneOutput.hpp"
#include "core/distribution/outputs/udpOutput.hpp"

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

  Tempo::AddSource(new ConstantTempo(120));

  hyp->hub.buttonPressed(0,0);

  hyp->start();

  while (1)
    Thread::sleep(1000);
}

#if ESP_PLATFORM
#define OUT_COLOR GRB
#define OUTPUT new NeopixelOutput(1)
#else
#define OUT_COLOR RGB
#define OUTPUT new MonitorOutput(wingMap)
#endif

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


              {.column = 1, .slot = 0, .pattern = new HorizontalGradientPattern(wingMap)},
              {.column = 1, .slot = 1, .pattern = new PixelGlitchPattern()},
              {.column = 1, .slot = 2, .pattern = new RadialFadePattern(wingMap)},
              {.column = 1, .slot = 3, .pattern = new ChevronsPattern(wingMap)},

              {.column = 2, .slot = 0, .pattern = new RibbenClivePattern<LFOPause<NegativeCosFast>>(10000, 1, 0.15)},
              {.column = 2, .slot = 1, .pattern = new RadialRainbowPattern(pWingMap)},
              {.column = 2, .slot = 2, .pattern = new RadialGradientPattern(pWingMap)},              
              {.column = 2, .slot = 3, .pattern = new StrobePattern()},

              {.column = 3, .slot = 0, .pattern = new FlashesPattern()},
              {.column = 3, .slot = 1, .pattern = new RadialGlitterFadePattern2(wingMap)},
              {.column = 3, .slot = 2, .pattern = new LineLaunch(wingMap)},

              {.column = 4, .slot = 0, .pattern = new SegmentChasePattern()},
              {.column = 4, .slot = 1, .pattern = new ClivePattern<LFOPause<SawDown>>(32,1000,1,0.1)},
              {.column = 4, .slot = 2, .pattern = new ClivePattern<SoftSquare>(32)},

              {.column = 5, .slot = 0, .pattern = new Lighthouse(pWingMap)},
              {.column = 5, .slot = 1, .pattern = new XY(wingMap)},

              {.column = 6, .slot = 0, .pattern = new GrowingCirclesPattern(wingMap)},
              {.column = 6, .slot = 1, .pattern = new GlowPulsePattern()},
              {.column = 6, .slot = 2, .pattern = new AngularFadePattern(pWingMap)},
              {.column = 6, .slot = 3, .pattern = new GrowingStrobePattern(pWingMap)},

          }),
      {60 * 8 * sizeof(RGBA),
       60 * 8 * sizeof(RGBA),
       60 * 8 * sizeof(RGBA),
       60 * 8 * sizeof(RGBA)},
      true);

  auto splitMap = PixelMapSplitter(
      &wingMap, {60 * 8, 60 * 8, 60 * 8, 60 * 8});


  const char* hosts[4] = {
    "hyperslave1.local",
    "hyperslave1.local",
    "hyperslave2.local",
    "hyperslave2.local"
  };

  int ports[4] = {
    9611,
    9695,
    9611,
    9615
  };

  for (int i = 0; i < splitInput->size(); i++)
  {
    auto pipe = new ConvertPipe<RGBA, RGB>(
        splitInput->getInput(i),
        //new MonitorOutput(splitMap.getMap(i)));
        new CloneOutput({
          new MonitorOutput(splitMap.getMap(i)),
          new UDPOutput(hosts[i], ports[i], 60)
        }));
    hyp->addPipe(pipe);
  }
}

void addPaletteColumn(Hyperion *hyp){
  auto paletteColumn = new PaletteColumn(&hyp->hub,0, std::vector<PaletteColumn::Palette>{
    coralTeal,
    //retro,
    //candy,
    greatBarrierReef,
    campfire,
    tunnel,
    {.gradient = &heatmap, .primary = heatmap.get(127), .secondary = heatmap.get(200), .highlight = heatmap.get(255) },
    {.gradient = &sunset1, .primary = sunset1.get(127), .secondary = sunset1.get(200), .highlight = sunset1.get(255) },
    {.gradient = &sunset3, .primary = sunset3.get(127), .secondary = sunset3.get(200), .highlight = sunset3.get(255) },
    {.gradient = &sunset4, .primary = sunset4.get(127), .secondary = sunset4.get(200), .highlight = sunset4.get(255) }
  });
  hyp->hub.subscribe(paletteColumn);
}
