
#include "core/hyperion.hpp"
#include "colours.h"
#include "distribution/inputs/patternInput.hpp"
#include "distribution/outputs/dmxOutput.hpp"
#include "generation/patterns/pattern.hpp"
#include "dmx.hpp"

#define SIZE 30

class WavePattern : public Pattern<Monochrome>
{
  LFO<SinFast> lfo;

  inline void Calculate(Monochrome *pixels, int width, bool firstFrame, Params *) override
  {
    for (int index = 0; index < width; index++)
      pixels[index] = 255 * lfo.getValue(float(index) / width);
  }
};

void pipe_example()
{
  auto hyp = new Hyperion();

  hyp->addPipe(
      new Pipe(
          new PatternInput<Monochrome>(SIZE, new WavePattern()),
          new DMXOutput(1)));

  hyp->start();
}

void raw_library_example()
{
  Log::info("HYP", "initializing");
  auto dmx = DMX::getInstance(0);
  // nb: if you dont send full frames, the buffer will only be updated on break signal of the next frame.
  // so the read value will behind by 1 loop cycle
  // dmx->sendFullFrame(false);
  uint8_t data[SIZE];
  uint8_t j = 0;

  while (1)
  {

    data[0] = j;
    for (int k = 0; k < SIZE - 1; k++)
    {
      data[k + 1] = data[k] + 1;
    }
    dmx->write(data, SIZE, 1);
    dmx->show();
    Thread::sleep(50); // it takes +/- 45ms for the full frame to be sent out
    uint8_t read_start = dmx->read(1);
    uint8_t read_end = dmx->read(SIZE);

    // be aware that data[index] is zero based, but dmx channels are 1 based
    Log::info("HYP", "written start: %d, read: %d", data[0], read_start);
    Log::info("HYP", "written end: %d, read: %d", data[SIZE - 1], read_end);

    Thread::sleep(10);
    j++;
  }
}

int main()
{
  pipe_example();
  // raw_library_example();
}