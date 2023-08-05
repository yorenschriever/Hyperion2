#include "core/distribution/inputs/inputSlicer.hpp"
#include "core/distribution/luts/colourCorrectionLut.hpp"
#include "core/distribution/luts/laserLut.hpp"
#include "core/distribution/luts/gammaLut.hpp"
#include "core/distribution/luts/incandescentLut.hpp"
#include "core/distribution/outputs/neopixelOutput.hpp"
#include "core/hyperion.hpp"
#include "distribution/inputs/patternCycleInput.hpp"
#include "core/distribution/inputs/inputSplitter.hpp"
#include "patterns.hpp"
#include <vector>

void addColanderPipe(Hyperion *hyp);
void addLaserPipe(Hyperion *hyp);
void addBulbPipe(Hyperion *hyp);
void addDMXPipe(Hyperion *hyp);

LUT *PixelLut = new ColourCorrectionLUT(1.5, 255, 255, 255, 240);
LUT *LaserLut = new LaserLUT(1.5, 4096, 2048);
LUT *IncandescentLut = new IncandescentLUT(2.5, 4096, 200);
LUT *GammaLut12 = new GammaLUT(2.5, 4096);

  const char *hosts[4] = {
      "hyperslave5.local",
      "hyperslave6.local",
      "hyperslave7.local",
      "hyperslave8.local" //"192.168.0.88"
    };

int main()
{
  auto hyp = new Hyperion();

  // hyp->hub.params.primaryColour = RGB(255, 0, 255);
  // hyp->hub.params.secondaryColour = RGB(50, 50, 50);
  // hyp->hub.params.highlightColour = RGB(250, 250, 250);
  // hyp->hub.params.gradient = &heatmap;



  // addWingsPipe(hyp);
  // addPaletteColumn(hyp);
  addColanderPipe(hyp);
  addLaserPipe(hyp);
  addBulbPipe(hyp);
  addDMXPipe(hyp);

  // hyp->hub.setFlashColumn(7);

  hyp->hub.setColumnName(1, "Vergieten");
  hyp->hub.setColumnName(2, "Lasers");
  hyp->hub.setColumnName(3, "Bulbs");
  hyp->hub.setColumnName(4, "DMX");

  Tempo::AddSource(new ConstantTempo(120));

  // hyp->hub.buttonPressed(0, 0);




  hyp->start();

  while (1)
    Thread::sleep(1000);
}

void addColanderPipe(Hyperion *hyp)
{

  auto splitInput = new InputSplitter(
      new ControlHubInput<Monochrome>(
          20,
          &hyp->hub,
          {
              // static
              {.column = 1, .slot = 0, .pattern = new GlowPattern()},
              {.column = 1, .slot = 1, .pattern = new SinPattern()},
          }),
      {5,5,5,5},
      true);

  for (int i = 0; i < splitInput->size(); i++)
  {
    auto pipe = new ConvertPipe<Monochrome, Monochrome12>(
        splitInput->getInput(i),
        new UDPOutput(hosts[i], 9620, 60),
        IncandescentLut
      );
    hyp->addPipe(pipe);
  }
}


void addLaserPipe(Hyperion *hyp)
{

  auto splitInput = new InputSplitter(
      new ControlHubInput<Monochrome>(
          12,
          &hyp->hub,
          {
              // static
              {.column = 2, .slot = 0, .pattern = new GlowPattern()},
              {.column = 2, .slot = 1, .pattern = new SinPattern()},
          }),
      {3,3,3,3},
      true);

  for (int i = 0; i < splitInput->size(); i++)
  {
    auto pipe = new ConvertPipe<Monochrome, Monochrome12>(
        splitInput->getInput(i),
        new UDPOutput(hosts[i], 9621, 60),
        LaserLut
      );
    hyp->addPipe(pipe);
  }
}

void addBulbPipe(Hyperion *hyp)
{

  auto splitInput = new InputSplitter(
      new ControlHubInput<Monochrome>(
          12,
          &hyp->hub,
          {
              // static
              {.column = 3, .slot = 0, .pattern = new GlowPattern()},
              {.column = 3, .slot = 1, .pattern = new SinPattern()},
          }),
      {3,3,3,3},
      true);

  for (int i = 0; i < splitInput->size(); i++)
  {
    auto pipe = new ConvertPipe<Monochrome, Monochrome12>(
        splitInput->getInput(i),
        new UDPOutput(hosts[i], 9622, 60),
        GammaLut12
      );
    hyp->addPipe(pipe);
  }
}

void addDMXPipe(Hyperion *hyp)
{

    auto input = new ControlHubInput<Monochrome>(
          20,
          &hyp->hub,
          {
              // static
              {.column = 4, .slot = 0, .pattern = new GlowPattern()},
              {.column = 4, .slot = 1, .pattern = new SinPattern()},
          });


    auto pipe = new ConvertPipe<Monochrome, Monochrome>(
      input,
      new UDPOutput("hyperslave3.local", 9619, 60)
    );

    hyp->addPipe(pipe);
}