#include "colours.h"
#include "core/distribution/inputs/inputSplitter.hpp"
#include "core/distribution/outputs/monitorOutput.hpp"
#include "core/distribution/pipes/convertPipe.hpp"
#include "core/generation/patterns/helpers/tempo/constantTempo.h"
#include "core/generation/pixelMap.hpp"
#include "core/generation/patterns/mappedPatterns.h"
#include "core/generation/pixelMapSplitter.hpp"
#include "core/hyperion.hpp"
#include "distribution/inputs/controlHubInput.hpp"
#include "mapping/columnMap.hpp"
#include "mapping/ledsterMap.hpp"
#include "palettes.hpp"
#include "patterns.hpp"
#include "platform/includes/thread.hpp"

auto pLedsterMap = ledsterMap.toPolarRotate90();
auto pColumnMap = columnMap.toPolarRotate90();

void addLedsterPipe(Hyperion *hyp);
void addColumnPipes(Hyperion *hyp);

int main()
{
  Params::primaryColour = RGB(255, 0, 255);
  Params::secondaryColour = RGB(50, 50, 50);
  Params::highlightColour = RGB(250, 250, 250);
  Params::palette = &heatmap; //&sunset1;

  auto hyp = new Hyperion();

  addColumnPipes(hyp);
  addLedsterPipe(hyp);

  Tempo::AddSource(ConstantTempo::getInstance());
  ConstantTempo::getInstance()->setBpm(120);

  hyp->start();
  while (1)
    Thread::sleep(1000);
}

// Pattern<RGBA> *patternLedster = new FWF::RadialGlitterFadePattern(pLedsterMap);
// Pattern<RGBA> *patternColumns = new FWF::RadialGlitterFadePattern(pColumnMap, 1./10);

// Pattern<RGBA> *patternLedster = new FWF::AngularFadePattern(pLedsterMap);
// Pattern<RGBA> *patternColumns = new FWF::AngularFadePattern(pColumnMap, 1./10);

// Pattern<RGBA> *patternLedster = new FWF::GlowPulsePattern();
// Pattern<RGBA> *patternColumns = new FWF::GlowPulsePattern();

// Pattern<RGBA> *patternLedster = new FWF::SquareGlitchPattern(ledsterMap);
// Pattern<RGBA> *patternColumns = new FWF::SquareGlitchPattern(columnMap);

Pattern<RGBA> *patternLedster = new FWF::GrowingStrobePattern(pLedsterMap);
Pattern<RGBA> *patternColumns = new FWF::GrowingStrobePattern(pColumnMap);

void addLedsterPipe(Hyperion *hyp)
{
  auto ledsterPipe = new ConvertPipe<RGBA, RGB>(
      new ControlHubInput<RGBA>(
          ledsterMap.size(),
          &hyp->hub,
          {
              {.column = 0, .slot = 0, .pattern = new FWF::RadialGlitterFadePattern(pLedsterMap)},
              {.column = 0, .slot = 1, .pattern = new FWF::AngularFadePattern(pLedsterMap)},
              {.column = 0, .slot = 2, .pattern = new FWF::GlowPulsePattern()},

              {.column = 1, .slot = 0, .pattern = new FWF::SquareGlitchPattern(ledsterMap)},
              {.column = 1, .slot = 1, .pattern = new FWF::GrowingStrobePattern(pLedsterMap)},
              {.column = 1, .slot = 2, .pattern = new Mapped::HorizontalGradientPattern(ledsterMap)},

              {.column = 2, .slot = 0, .pattern = new Mapped::ConcentricWavePattern<SinFast>(ledsterMap)},
              {.column = 2, .slot = 1, .pattern = new Mapped::HorizontalGradientPattern(ledsterMap)},
              {.column = 2, .slot = 2, .pattern = new Mapped::ConcentricWavePattern<SinFast>(ledsterMap)},

              {.column = 3, .slot = 0, .pattern = new Mapped::HorizontalGradientPattern(ledsterMap)},
              {.column = 3, .slot = 1, .pattern = new Mapped::ConcentricWavePattern<SinFast>(ledsterMap)},
              {.column = 3, .slot = 2, .pattern = new Mapped::HorizontalGradientPattern(ledsterMap)},
          }),
      new MonitorOutput(ledsterMap));
  hyp->addPipe(ledsterPipe);
}

void addColumnPipes(Hyperion *hyp)
{
  // Generate 1 pattern, and split it up in six outputs,
  // because UDPOutput (and therefore MonitorOutput) are limited by a
  // maximum transfer size of 2*1440 bytes
  auto splitInput = new InputSplitter(
      new ControlHubInput<RGBA>(
          columnMap.size(),
          &hyp->hub,
          {
              {.column = 0, .slot = 0, .pattern = new FWF::RadialGlitterFadePattern(pColumnMap)},
              {.column = 0, .slot = 1, .pattern = new FWF::AngularFadePattern(pColumnMap)},
              {.column = 0, .slot = 2, .pattern = new FWF::GlowPulsePattern()},

              {.column = 1, .slot = 0, .pattern = new FWF::SquareGlitchPattern(columnMap)},
              {.column = 1, .slot = 1, .pattern = new FWF::GrowingStrobePattern(pColumnMap)},
              {.column = 1, .slot = 2, .pattern = new Mapped::HorizontalGradientPattern(columnMap)},

              {.column = 2, .slot = 0, .pattern = new Mapped::ConcentricWavePattern<SinFast>(columnMap)},
              {.column = 2, .slot = 1, .pattern = new Mapped::HorizontalGradientPattern(columnMap)},
              {.column = 2, .slot = 2, .pattern = new Mapped::ConcentricWavePattern<SinFast>(columnMap)},

              {.column = 3, .slot = 0, .pattern = new Mapped::HorizontalGradientPattern(columnMap)},
              {.column = 3, .slot = 1, .pattern = new Mapped::ConcentricWavePattern<SinFast>(columnMap)},
              {.column = 3, .slot = 2, .pattern = new Mapped::HorizontalGradientPattern(columnMap)},
          }),
      {480 * sizeof(RGBA),
       480 * sizeof(RGBA),
       480 * sizeof(RGBA),
       480 * sizeof(RGBA),
       480 * sizeof(RGBA),
       480 * sizeof(RGBA)},
      true);

  auto splitMap = PixelMapSplitter(
      &columnMap, {480, 480, 480, 480, 480, 480});

  for (int i = 0; i < splitInput->size(); i++)
  {
    auto pipe = new ConvertPipe<RGBA, RGB>(
        splitInput->getInput(i),
        new MonitorOutput(splitMap.getMap(i)));
    hyp->addPipe(pipe);
  }
}