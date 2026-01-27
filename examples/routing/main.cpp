#include "hyperion.hpp"

class ColorPattern : public Pattern<RGB>
{
public:
  ColorPattern(RGB col) : col(col) {}

  void Calculate(RGB *pixels, int width, bool firstFrame, Params *) override
  {
    for (int index = 0; index < width; index++)
      pixels[index] = col;
  }

private:
  RGB col;
};

int main()
{
  auto hyp = new Hyperion();

  auto inputRed =   new PatternInput(10, new ColorPattern(RGB(255, 0, 0)));
  auto inputGreen = new PatternInput(10, new ColorPattern(RGB(0, 255, 0)));
  auto inputBlue =  new PatternInput(10, new ColorPattern(RGB(0, 0, 255)));

  auto combine = new Combine();

  hyp->createChain(inputRed,   combine->atOffset(0));
  hyp->createChain(inputGreen, combine->atOffset(30));
  hyp->createChain(inputBlue,  combine->atOffset(60 + 30));

  auto slicer = new Slicer({
    {0, 120, true},
    {0, 120}
  });

  hyp->createChain(combine,slicer);

  auto map = new PixelMap(gridMap(40,1));
  hyp->createChain(slicer->getSlice(0), new UDPOutput("hypernode1.local",9611));
  hyp->createChain(slicer->getSlice(1), new MonitorOutput(&hyp->webServer, map));

  hyp->start();

  while (true)
    Thread::sleep(10);
}