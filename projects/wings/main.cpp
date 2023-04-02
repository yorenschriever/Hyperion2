#include "colours.h"
#include "core/distribution/inputs/patternInput.hpp"
#include "core/distribution/luts/colourCorrectionLut.hpp"
#include "core/distribution/outputs/monitorOutput.hpp"
#include "core/distribution/outputs/neopixelOutput.hpp"
#include "core/distribution/pipes/convertPipe.hpp"
#include "core/generation/patterns/helpers/tempo/constantTempo.h"
#include "core/hyperion.hpp"
#include "core/generation/pixelMap.hpp"
#include "mapping/wingMap.hpp"
#include "ledsterPatterns.h"
#include "core/generation/pixelMapSplitter.hpp"
#include "core/distribution/inputs/inputSplitter.hpp"

void addWingsPipe(Hyperion *hyp);

LUT *PixelLut = new ColourCorrectionLUT(1.5, 255, 255, 255, 240);

int main()
{
  auto hyp = new Hyperion();

  hyp->hub.params.primaryColour = RGB(255, 0, 255);
  hyp->hub.params.secondaryColour = RGB(250, 210, 0);

  addWingsPipe(hyp);

  Tempo::AddSource(ConstantTempo::getInstance());
  ConstantTempo::getInstance()->setBpm(120);

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
      new PatternInput<RGBA>(
          wingMap.size(),
          new Ledster::ChevronsPattern(wingMap)
          ),
      {60 * 8 * sizeof(RGBA),
       60 * 8 * sizeof(RGBA),
       60 * 8 * sizeof(RGBA),
       60 * 8 * sizeof(RGBA)},
      true);

  auto splitMap = PixelMapSplitter(
      &wingMap, {60 * 8,60 * 8,60 * 8,60 * 8});

  for (int i = 0; i < splitInput->size(); i++)
  {
    auto pipe = new ConvertPipe<RGBA, RGB>(
        splitInput->getInput(i),
        new MonitorOutput(splitMap.getMap(i)));
    hyp->addPipe(pipe);
  }
}
