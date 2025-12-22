#include "hyperion.hpp"

// send a test signal to a hypernode to test if the hardware is working

class RainbowPattern : public Pattern<RGB>
{
  inline void Calculate(RGB *pixels, int width, bool firstFrame, Params*) override
  {
    for (int index = 0; index < width; index++)
    {
      pixels[index] = Hue(0xFF * index / width + (Utils::millis() / 10)) ;
    }
  }
};

const char *target = "hypernode1.local";

int main()
{
  auto hyp = new Hyperion();

  for(int i=1;i<=8;i++){
    hyp->createChain(
      new PatternInput(100,new RainbowPattern()),
      new UDPOutput(target,9610+i,60)
    );
  }


  hyp->createChain(
    new PatternInput(512,new RainbowPattern()),
    new UDPOutput(target,9619,30)
  );


  hyp->start(Hyperion::minimal);

  while(true) Thread::sleep(10);
}