
#include "core/hyperion.hpp"
#include "colours.h"
#include "distribution/inputs/patternInput.hpp"
#include "distribution/outputs/pwmOutput.hpp"
#include "generation/patterns/pattern.hpp"
#include "distribution/luts/gammaLut.hpp"
#include "dmx.hpp"

#define SIZE 12

LUT *GammaLut12 = new GammaLUT(2.5, 4096);

class WavePattern : public Pattern<Monochrome>
{
  LFO<SinFast> lfo;

  inline void Calculate(Monochrome *pixels, int width, bool firstFrame, Params *) override
  {
    for (int index = 0; index < width; index++)
      pixels[index] = 255 * lfo.getValue(float(index) / width);
  }
};

int main()
{
  auto hyp = new Hyperion();

  hyp->addPipe(
      new ConvertPipe<Monochrome,Monochrome12>(
          new PatternInput<Monochrome>(SIZE, new WavePattern()),
          new PWMOutput(1),
          GammaLut12));

  hyp->start();

  while(true) Thread::sleep(10);
}