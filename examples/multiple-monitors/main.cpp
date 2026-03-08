#include "hyperion.hpp"

class ColorPattern : public Pattern<RGBA>
{
public:
  ColorPattern(RGBA col, const char* name) : col(col), name(name) {}

  void Calculate(RGBA *pixels, int width, bool active, Params *) override
  {
    if (!active)
      return;
    for (int index = 0; index < width; index++)
      pixels[index] = col;
  }

private:
  RGBA col;
  const char* name;
};

void instance1(Hyperion* hyp) {
  auto map = new PixelMap(circleMap(20, 0.8));
  hyp->createChain(
    new ControlHubInput<RGBA>(
          map->size(),
          &hyp->hub,
          {
            {.column = 0, .slot = 0, .pattern = new ColorPattern(RGBA(255, 0, 0, 255), "Red")},
            {.column = 0, .slot = 1, .pattern = new ColorPattern(RGBA(0, 255, 0, 255), "Green")},
            {.column = 0, .slot = 2, .pattern = new ColorPattern(RGBA(0, 0, 255, 255), "Blue")},
          }),
    new ColorConverter<RGBA, RGB>(),
    new MonitorOutput(&hyp->webServer,map,"Instance 1", 60, 0.1));
}

void instance2(Hyperion* hyp) {
  auto map = new PixelMap(gridMap(5,5));
  hyp->createChain(
    new ControlHubInput<RGBA>(
          map->size(),
          &hyp->hub,
          {
            {.column = 1, .slot = 0, .pattern = new ColorPattern(RGBA(255, 0, 0, 255), "Red")},
            {.column = 1, .slot = 1, .pattern = new ColorPattern(RGBA(0, 255, 0, 255), "Green")},
            {.column = 1, .slot = 2, .pattern = new ColorPattern(RGBA(0, 0, 255, 255), "Blue")},
          }),
    new ColorConverter<RGBA, RGB>(),
    new MonitorOutput(&hyp->webServer,map,"Instance 2", 60, 0.1));
}

int main()
{
  auto hyp = new Hyperion();

  instance1(hyp);
  instance2(hyp);
  hyp->start();

  while (true)
    Thread::sleep(10);
}