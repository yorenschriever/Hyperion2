#include "hyperion.hpp"
#include "mapping/bulbMap.hpp"
#include "mapping/ledMap.hpp"
#include "ledPatterns.hpp"
#include "patterns-monochrome.hpp"
#include "patterns-monochrome-mapped.hpp"

LUT *incandescentLut8 = new IncandescentLUT(2.5, 255, 24);

void addLedPipe(Hyperion *hyp);
void addBulbPipe(Hyperion *hyp);

#define COL_BULBS 0
#define COL_BULBS_MAPPED 1
#define COL_LEDS 2

PixelMap::Polar pBulbMap = bulbMap.toPolar();

int main()
{
  auto hyp = new Hyperion();

  addBulbPipe(hyp);
  addLedPipe(hyp);

  hyp->hub.setColumnName(COL_BULBS, "Bulbs");
  hyp->hub.setColumnName(COL_BULBS_MAPPED, "Bulbs mapped");
  hyp->hub.setColumnName(COL_LEDS, "Leds");

  hyp->start();

  while (1)
    Thread::sleep(1000);
}

void addBulbPipe(Hyperion *hyp)
{
  auto input = new ControlHubInput<Monochrome>(
      bulbMap.size(),
      &hyp->hub,
      {
            {.column = COL_BULBS, .slot = 0, .pattern = new MonochromePatterns::BlinderPattern()},
            {.column = COL_BULBS, .slot = 1, .pattern = new MonochromePatterns::SinPattern()},
            {.column = COL_BULBS, .slot = 2, .pattern = new MonochromePatterns::GlowPattern()},
            {.column = COL_BULBS, .slot = 3, .pattern = new MonochromePatterns::LFOPattern<SoftSawDown>("Soft Saw Down")},
            {.column = COL_BULBS, .slot = 4, .pattern = new MonochromePatterns::BeatShakePattern()},
            {.column = COL_BULBS, .slot = 5, .pattern = new MonochromePatterns::BeatMultiFadePattern()},
            {.column = COL_BULBS, .slot = 6, .pattern = new MonochromePatterns::LFOPattern<Glow>("Neg Cos")},
            {.column = COL_BULBS, .slot = 7, .pattern = new MonochromePatterns::BeatSingleFadePattern()},

            {.column = COL_BULBS, .slot = 8, .pattern = new MonochromePatterns::LFOPattern<Sin>("Sin")},
            {.column = COL_BULBS, .slot = 9, .pattern = new MonochromePatterns::LFOPattern<PWM>("PWM")},
            {.column = COL_BULBS, .slot = 10, .pattern = new MonochromePatterns::BeatStepPattern()},
            {.column = COL_BULBS, .slot = 11, .pattern = new MonochromePatterns::SlowStrobePattern()},
            {.column = COL_BULBS, .slot = 12, .pattern = new MonochromePatterns::BeatAllFadePattern()},
            {.column = COL_BULBS, .slot = 13, .pattern = new MonochromePatterns::OnPattern()},

            {.column = COL_BULBS, .slot = 14, .pattern = new MonochromePatterns::OnPattern()},
            {.column = COL_BULBS, .slot = 15, .pattern = new MonochromePatterns::GlitchPattern()},
            {.column = COL_BULBS, .slot = 16, .pattern = new MonochromePatterns::BeatAllFadePattern()},

            {.column = COL_BULBS_MAPPED, .slot = 0, .pattern = new MonochromeMappedPatterns::SinPattern(&bulbMap)},
            {.column = COL_BULBS_MAPPED, .slot = 1, .pattern = new MonochromeMappedPatterns::RadialFadePattern(&pBulbMap)},

      });

  auto pipe = new ConvertPipe<Monochrome, RGB>(
      input,
      new MonitorOutput(&hyp->webServer, &bulbMap, 60, .03));

  hyp->addPipe(pipe);
}

void addLedPipe(Hyperion *hyp)
{
  auto input = new ControlHubInput<RGBA>(
      ledMap.size(),
      &hyp->hub,
      {
          {.column = COL_LEDS, .slot = 0, .pattern = new LedPatterns::OnPattern(RGB(255,0,0), "red")},
      });

  auto pipe = new ConvertPipe<RGBA, RGB>(
      input,
      new MonitorOutput(&hyp->webServer, &ledMap));

  hyp->addPipe(pipe);
}

