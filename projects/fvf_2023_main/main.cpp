#include "colours.h"
#include "core/distribution/inputs/inputSplitter.hpp"
#include "core/distribution/outputs/monitorOutput.hpp"
#include "core/distribution/outputs/monitorOutput3d.hpp"
#include "core/distribution/pipes/convertPipe.hpp"
#include "core/generation/patterns/helpers/tempo/constantTempo.h"
#include "core/generation/pixelMap.hpp"
#include "core/generation/patterns/mappedPatterns.h"
#include "core/generation/pixelMapSplitter.hpp"
#include "core/generation/pixelMapSplitter3d.hpp"
#include "core/hyperion.hpp"
#include "distribution/inputs/controlHubInput.hpp"
#include "mapping/columnMap.hpp"
#include "mapping/ledsterMap.hpp"
#include "mapping/columnMap3d.hpp"
#include "mapping/ledsterMap3d.hpp"
#include "palettes.hpp"
#include "patterns.hpp"
#include "ledsterPatterns.hpp"
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
              {.column = 1, .slot = 2, .pattern = new Ledster::RadialFadePattern(ledsterMap)},

              {.column = 2, .slot = 0, .pattern = new Ledster::ClivePattern<SinFast>(10000)},
              {.column = 2, .slot = 1, .pattern = new Ledster::RibbenClivePattern<LFOPause<NegativeCosFast>>(10000,1,0.25)},
              {.column = 2, .slot = 2, .pattern = new Ledster::SnakePattern()},

              {.column = 3, .slot = 0, .pattern = new Ledster::RibbenClivePattern<SoftSquare>(40000)},
              {.column = 3, .slot = 1, .pattern = new Ledster::RibbenClivePattern<SawDown>(500)},
              {.column = 3, .slot = 2, .pattern = new Ledster::RibbenClivePattern<SinFast>(3000)},

              {.column = 4, .slot = 0, .pattern = new Ledster::RibbenClivePattern<LFOPause<NegativeCosFast>>(10000,1,0.15)},
              {.column = 4, .slot = 1, .pattern = new Ledster::RibbenClivePattern<LFOPause<SawDown>>(10000,1,0.15)},
              {.column = 4, .slot = 2, .pattern = new Ledster::RibbenClivePattern<PWM>(10000,1,0.0025)},


              {.column = 5, .slot = 0, .pattern = new FWF::RibbenFlashPattern()},
              {.column = 5, .slot = 1, .pattern = new Ledster::ChevronsPattern(ledsterMap),},
              {.column = 5, .slot = 2, .pattern = new Ledster::PixelGlitchPattern()},
          }),
      new MonitorOutput3d(ledsterMap3d));
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

              {.column = 2, .slot = 0, .pattern = new Mapped::HorizontalGradientPattern(columnMap)},
              {.column = 2, .slot = 1, .pattern = new FWF::RibbenClivePattern<LFOPause<NegativeCosFast>>(10000,1,0.25)},
              {.column = 2, .slot = 2, .pattern = new Mapped::HorizontalGradientPattern(columnMap)},

              {.column = 3, .slot = 0, .pattern = new FWF::RibbenClivePattern<SoftSquare>(40000)},
              {.column = 3, .slot = 1, .pattern = new FWF::RibbenClivePattern<SawDown>(500)},
              {.column = 3, .slot = 2, .pattern = new FWF::RibbenClivePattern<SinFast>(3000)},

              {.column = 4, .slot = 0, .pattern = new FWF::RibbenClivePattern<LFOPause<NegativeCosFast>>(10000,1,0.15)},
              {.column = 4, .slot = 1, .pattern = new FWF::RibbenClivePattern<LFOPause<SawDown>>(10000,1,0.15)},
              {.column = 4, .slot = 2, .pattern = new FWF::RibbenClivePattern<PWM>(10000,1,0.0025)},

              {.column = 5, .slot = 0, .pattern = new FWF::RibbenFlashPattern()},
              {.column = 5, .slot = 1, .pattern = new Ledster::ChevronsPattern(columnMap)},
              {.column = 5, .slot = 2, .pattern = new Ledster::PixelGlitchPattern()},
          }),
      {480 * sizeof(RGBA),
       480 * sizeof(RGBA),
       480 * sizeof(RGBA),
       480 * sizeof(RGBA),
       480 * sizeof(RGBA),
       480 * sizeof(RGBA)},
      true);

  auto splitMap = PixelMapSplitter3d(
      &columnMap3d, {480, 480, 480, 480, 480, 480});

  for (int i = 0; i < splitInput->size(); i++)
  {
    auto pipe = new ConvertPipe<RGBA, RGB>(
        splitInput->getInput(i),
        new MonitorOutput3d(splitMap.getMap(i)));
    hyp->addPipe(pipe);
  }
}