#include "hyperion.hpp"

class ColorPattern : public Pattern<RGBA>
{
public:
  ColorPattern(RGBA col, const char* name) {
    this->col = col;
    this->name = name;
  }

  void Calculate(RGBA *pixels, int width, bool active, Params *) override
  {
    if (!active)
      return;
    for (int index = 0; index < width; index++)
      pixels[index] = col;
  }

private:
  RGBA col;
};

int main()
{
  auto hyp = new Hyperion();

  auto slotPatterns = std::vector<ControlHubInput<RGBA>::SlotPattern>{
    {.column = 0, .slot = 0, .pattern = new ColorPattern(RGBA(255, 0, 0, 255), "Red")},
    {.column = 0, .slot = 1, .pattern = new ColorPattern(RGBA(0, 255, 0, 255), "Green")},
    {.column = 0, .slot = 2, .pattern = new ColorPattern(RGBA(0, 0, 255, 255), "Blue")},
  };

  auto map = new PixelMap(circleMap(20, 0.8));
  hyp->createChain(
    new ControlHubInput<RGBA>(map->size(), &hyp->hub, slotPatterns),
    new ColorConverter<RGBA, RGB>(),
    new MonitorOutput(&hyp->webServer,map,nullptr, 60, 0.1));

  hyp->createChain(
    (new ControlHubInput<RGBA>(map->size(), &hyp->hub, slotPatterns))->setActivatedMask(ControlHub::PREVIEW),
    new ColorConverter<RGBA, RGB>(),
    new MonitorOutput(&hyp->webServer,map,"Preview", 60, 0.1));

  hyp->start();

  while (true)
    Thread::sleep(10);
}