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

  auto inputRed = new PatternInput(10, new ColorPattern(RGB(255, 0, 0)));
  auto inputGreen = new PatternInput(10, new ColorPattern(RGB(0, 255, 0)));
  auto inputBlue = new PatternInput(10, new ColorPattern(RGB(0, 0, 255)));

  auto combine = new Combine();

  hyp->createChain(inputRed, combine->atOffset(0));
  hyp->createChain(inputRed, combine->atOffset(30));
  hyp->createChain(inputRed, combine->atOffset(60 + 10));

  auto map = gridMap(100,1);
  hyp->createChain(combine, new MonitorOutput(hyp->webServer, map)));

  // auto sw = new Switch(2);

  // hyp->createPartialChain(
  //   new PatternInput(100,new RainbowPattern()),
  //   sw->getReceiver(0)
  // );

  // hyp->createPartialChain(
  //   new PatternInput(100,new RainbowPattern()),
  //   sw->getReceiver(0)
  // );

  // hyp->createPartialChain(
  //   sw,
  //   new UDPOutput(target,9619,30)
  // );

  // ISender input =
  //       new Switch(
  //           input1,
  //           input2,
  //           [](){return hyp->hub.findSlot(autoColumn,0)->activated;}
  //       );

  hyp->start();

  while (true)
    Thread::sleep(10);
}