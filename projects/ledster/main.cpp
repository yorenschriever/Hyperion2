#include "colours.h"
#include "core/distribution/inputs/patternInput.hpp"
#include "core/distribution/luts/colourCorrectionLut.hpp"
#include "core/distribution/outputs/monitorOutput.hpp"
#include "core/distribution/outputs/neopixelOutput.hpp"
#include "core/distribution/pipes/convertPipe.hpp"
#include "core/generation/patterns/helpers/tempo/constantTempo.h"
#include "core/generation/pixelMap.hpp"
#include "core/hyperion.hpp"
#include "ledsterPatterns.h"
#include "mapping/ledsterMap.hpp"

void addLedsterPipe(Hyperion *hyp);

LUT *PixelLut = new ColourCorrectionLUT(1.5, 255, 255, 255, 240);

int main()
{
  Params::primaryColour = RGB(255, 0, 255);
  Params::secondaryColour = RGB(250, 210, 0);
  // Params::intensity = 1;
  // Params::variant= 0.7;
  // Params::velocity = 0.8;

  Params::intensity = 0.5;
  Params::variant = 0.5;
  Params::velocity = 0.5;

  auto hyp = new Hyperion();

  addLedsterPipe(hyp);

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
#define OUTPUT new MonitorOutput(ledsterMap)
#endif

void addLedsterPipe(Hyperion *hyp)
{
  auto ledsterPipe = new ConvertPipe<RGBA, OUT_COLOR>(
      new PatternInput<RGBA>(
          ledsterMap.size(),

          // new Ledster::SnowflakePattern()
          // new Ledster::RadialGlitterFadePattern(ledsterMap)
          // new Ledster::RibbenClivePattern<SoftSquare>()
          // new Ledster::SnakePattern()
          // new Ledster::RadarPattern(ledsterMap)

          // new Ledster::PetalChasePattern()
          //  new Ledster::RadialFadePattern(ledsterMap)
          //  new Ledster::RibbenClivePattern<SoftSquare>()
          //  new Ledster::SnowflakePattern()
          //  new Ledster::PetalRotatePattern()
          // new Ledster::ClivePattern<LFOPause<SinFast>>(ledsterMap.size(), 10000, 5, 0.25)

          // new Ledster::RadarPattern(ledsterMap)
          // new Ledster::ChevronsPattern(ledsterMap)
          new Ledster::RadialRainbowPattern(ledsterMap)

          // //mapped
          // new Ledster::RadialFadePattern(ledsterMap)
          // new Ledster::RadialGlitterFadePattern(ledsterMap)
          // new Ledster::KonamiFadePattern(ledsterMap),

          // //inner
          // //new Ledster::AdidasPattern(),
          // //new Ledster::ConcentricChaserPattern(),
          // //new Ledster::MandalaPattern(),
          // new Ledster::ClivePattern<SinFast>(10000),
          // new Ledster::RadialFadePattern2(ledsterMap),
          // new Ledster::DoorPattern(ledsterMap),

          // //outer
          // new Ledster::PetalChasePattern(),
          // new Ledster::PetalRotatePattern(),
          // new Ledster::PetalGlitchPattern(),

          // //sparse
          // new Ledster::SnowflakePattern(),
          // new Ledster::SnakePattern()
          // new Ledster::HexBeatPattern()

          // //clive
          // new Ledster::RibbenClivePattern<SoftSquare>(40000)
          // new Ledster::RibbenClivePattern<SawDown>(500)
          // new Ledster::RibbenClivePattern<SinFast>(3000)
          ),
      OUTPUT,
      PixelLut);
  hyp->addPipe(ledsterPipe);
}
