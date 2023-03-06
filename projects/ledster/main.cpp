#include "colours.h"
#include "core/distribution/inputs/patternInput.hpp"
#include "core/distribution/outputs/monitorOutput.hpp"
#include "core/distribution/outputs/neopixelOutput.hpp"
#include "core/distribution/pipes/convertPipe.hpp"
#include "core/hyperion.hpp"
#include "core/generation/pixelMap.hpp"
#include "mapping/ledsterMap.hpp"
#include "ledsterPatterns.h"
#include "core/generation/patterns/helpers/tempo/constantTempo.h"

void addLedsterPipe(Hyperion *hyp);

int main()
{
  Params::primaryColour = RGB(255, 0, 255);
  Params::secondaryColour = RGB(50, 10, 0);
  Params::intensity = 1;
  Params::variant= 0.7;
  Params::velocity = 0.7;

  auto hyp = Hyperion();

  addLedsterPipe(&hyp);

  Tempo::AddSource(ConstantTempo::getInstance());
  ConstantTempo::getInstance()->setBpm(120);

  hyp.setup();
  while (1){
    hyp.run();
    Thread::sleep(12);
  }
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

          //new Ledster::SnowflakePattern()
          //new Ledster::RadialGlitterFadePattern(ledsterMap)
          //new Ledster::RibbenClivePattern<SoftSquare>()
          new Ledster::SnakePattern()
          //new Ledster::RadarPattern(ledsterMap)

          //new Ledster::PetalChasePattern()
          // new Ledster::RadialFadePattern(ledsterMap)
          // new Ledster::RibbenClivePattern<SoftSquare>()
          // new Ledster::SnowflakePattern()
          // new Ledster::PetalRotatePattern()
          //new Ledster::ClivePattern<LFOPause<SinFast>>(numLeds, 10000, 5, 0.25)}
      ),
      OUTPUT
    );
  hyp->addPipe(ledsterPipe);
}
