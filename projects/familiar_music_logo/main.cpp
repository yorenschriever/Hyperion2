#include "hyperion.hpp"
#include "mapping/bulbMap.hpp"
#include "mapping/ledMap.hpp"
#include "bulbPatterns.hpp"
#include "ledPatterns.hpp"

LUT *incandescentLut8 = new IncandescentLUT(2.5, 255, 24);

void addLedPipe(Hyperion *hyp);
void addBulbPipe(Hyperion *hyp);

#define COL_BULBS 0
#define COL_LEDS 1

int main()
{
  auto hyp = new Hyperion();

  addBulbPipe(hyp);
  addLedPipe(hyp);

  hyp->hub.setColumnName(COL_BULBS, "Bulbs");
  hyp->hub.setColumnName(COL_LEDS, "Leds");

  hyp->start();

  while (1)
    Thread::sleep(1000);
}

void addBulbPipe(Hyperion *hyp)
{
  auto input = new ControlHubInput<Monochrome>(
      ledMap.size(),
      &hyp->hub,
      {
          {.column = COL_BULBS, .slot = 0, .pattern = new BulbPatterns::OnPattern()},
      });

  auto pipe = new ConvertPipe<Monochrome, Monochrome>(
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

