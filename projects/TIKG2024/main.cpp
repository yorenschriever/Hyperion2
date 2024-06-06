#include "core/distribution/inputs/inputSlicer.hpp"
#include "core/distribution/inputs/inputSplitter.hpp"
#include "core/distribution/luts/colourCorrectionLut.hpp"
#include "core/distribution/luts/gammaLut.hpp"
#include "core/distribution/luts/incandescentLut.hpp"
#include "core/distribution/luts/laserLut.hpp"
#include "core/distribution/outputs/monitorOutput.hpp"
#include "core/distribution/outputs/neopixelOutput.hpp"
#include "core/hyperion.hpp"
#include "distribution/inputs/patternCycleInput.hpp"
#include "core/generation/palettes.hpp"
#include "patterns.hpp"
#include <vector>
#include "mapping/ledMap.hpp"
#include "ledPatterns.hpp"
#include "core/generation/pixelMapSplitter3d.hpp"
#include "core/distribution/inputs/inputSlicer.hpp"
#include "mask.hpp"

#define COL_PALETTE 0
#define COL_LED1 1
#define COL_LED2 2
#define COL_LED3 3
#define COL_LED4 4


void addPaletteColumn(Hyperion *hyp);
void addLed1Column(Hyperion *hyp);
//void addLed2Column(Hyperion *hyp);

// LUT *PixelLut = new ColourCorrectionLUT(1.5, 255, 255, 255, 240);
// LUT *LaserLut = new LaserLUT(0.5, 4096, 3048);
// LUT *IncandescentLut = new IncandescentLUT(2.5, 4096, 200);
// LUT *IncandescentLut8 = new IncandescentLUT(2.5, 255, 24);
// LUT *GammaLut12 = new GammaLUT(2.5, 4096);
// LUT *GammaLut8 = new GammaLUT(2.5, 255);
LUT *ledLut = new ColourCorrectionLUT(2.7, 255, 255, 255, 255);

int main()
{
  auto hyp = new Hyperion();

  //hyp->setMidiControllerFactory(new DinMidiControllerFactory<ApcMiniController>());

  addLed1Column(hyp);

  addPaletteColumn(hyp);

  hyp->hub.setColumnName(COL_PALETTE, "Kleur");
  hyp->hub.setColumnName(COL_LED1, "LED 1");
  hyp->hub.setColumnName(COL_LED2, "LED 2");
  hyp->hub.setColumnName(COL_LED3, "LED 3");
  hyp->hub.setColumnName(COL_LED4, "LED 4");

// tempo voor tijdens het programmeren
   //Tempo::AddSource(new ConstantTempo(120));

  for (int i = 1; i < 10; i++)
    hyp->hub.setFlashColumn(i, false, true);
  
  hyp->hub.setFlashColumn(4,true,true);
  //hyp->hub.setFlashRow(5);
  //hyp->hub.setFlashRow(6);
  //hyp->hub.setFlashRow(7);

  hyp->hub.buttonPressed(COL_PALETTE, 0);
  hyp->hub.setForcedSelection(COL_PALETTE);
  hyp->hub.setFlashColumn(COL_PALETTE,false,true);

  hyp->start();

  while (1)
    Thread::sleep(1000);
}


void addLed1Column(Hyperion *hyp)
{
    int nleds = 4*8*60;
    int nbytes = nleds * sizeof(RGBA);
    int i=0;

    auto input = new ControlHubInput<RGBA>(
        nleds,
        &hyp->hub,
        {
            {.column = COL_LED1, .slot = 0, .pattern = new Patterns::CeilingChase()},
            {.column = COL_LED1, .slot = 1, .pattern = new Patterns::Fireworks()},
            {.column = COL_LED1, .slot = 2, .pattern = new Patterns::Chaser()},
            {.column = COL_LED1, .slot = 3, .pattern = new Patterns::SegmentChasePattern()},
            {.column = COL_LED1, .slot = 4, .pattern = new Patterns::FlashesPattern()},
            {.column = COL_LED1, .slot = 5, .pattern = new Patterns::GlowPulsePattern()},
            {.column = COL_LED1, .slot = 6, .pattern = new Patterns::BarLFO()},
            {.column = COL_LED1, .slot = 7, .pattern = new Patterns::GradientLFO()},
            
            {.column = COL_LED2, .slot = 0, .pattern = new Patterns::FadeFromRandom()},
            {.column = COL_LED2, .slot = 1, .pattern = new Patterns::RibbenClivePattern<NegativeCosFast>()},
            {.column = COL_LED2, .slot = 2, .pattern = new Patterns::SinChasePattern()},
            {.column = COL_LED2, .slot = 3, .pattern = new Patterns::SawChasePattern()},
            {.column = COL_LED2, .slot = 4, .pattern = new Patterns::FadeFromCenter()},
            {.column = COL_LED2, .slot = 5, .pattern = new Patterns::SideWave()},
            {.column = COL_LED2, .slot = 6, .pattern = new Patterns::SinChase2Pattern()},
            
            {.column = COL_LED3, .slot = 0, .pattern = new Patterns::ColumnChaserTIKG()},
            {.column = COL_LED3, .slot = 1, .pattern = new Patterns::ColumnDimmerTIKG()},
            {.column = COL_LED3, .slot = 3, .pattern = new Patterns::GlowPulseMaskPattern()},
            {.column = COL_LED3, .slot = 4, .pattern = new Patterns::SideWaveMask(0)},
            {.column = COL_LED3, .slot = 5, .pattern = new Patterns::SinChaseMaskPattern(0)},


            {.column = COL_LED4, .slot = 0, .pattern = new Patterns::StrobeHighlightPattern()},
            {.column = COL_LED4, .slot = 1, .pattern = new Patterns::PixelGlitchPattern()},
            {.column = COL_LED4, .slot = 2, .pattern = new Patterns::SegmentGlitchPattern()},
            {.column = COL_LED4, .slot = 3, .pattern = new Patterns::StrobePattern()},
            {.column = COL_LED4, .slot = 4, .pattern = new Patterns::GlitchPattern()},
            {.column = COL_LED4, .slot = 5, .pattern = new Patterns::FadingNoisePattern()},
            {.column = COL_LED4, .slot = 6, .pattern = new Patterns::RibbenFlashPattern()},
            

        });

    auto splitInput = new InputSlicer(
        input,
        {
            {0 * nbytes / 4, nbytes / 4, true},
            {1 * nbytes / 4, nbytes / 4, true},
            {2 * nbytes / 4, nbytes / 4, true},
            {3 * nbytes / 4, nbytes / 4, true},
            {0, nbytes, false},
        });

    for (int i = 0; i < splitInput->size() - 1; i++)
    {
        auto pipe = new ConvertPipe<RGBA, BGR>(
            splitInput->getInput(i),
            // new NeopixelOutput(i+1),
            new UDPOutput("hyperslave4.local",9611 + i,60),
            ledLut);
        hyp->addPipe(pipe);
    }

    hyp->addPipe(
        new ConvertPipe<RGBA, RGB>(
            splitInput->getInput(4),
            new MonitorOutput(&hyp->webServer, &ledMap1)));
}


void addPaletteColumn(Hyperion *hyp)
{
  auto paletteColumn = new PaletteColumn(
      &hyp->hub,
      0,
      0,
      {
          &pinkSunset,
          &blueOrange,
          &purpleGreen,
          &heatmap,
          &retro,
          &campfire,
          &sunset4,
          &tunnel,


          &sunset2,
          &sunset8,
          &heatmap2,
          &sunset3,
          &sunset6,
          &sunset7,
          &coralTeal,
          &deepBlueOcean,
          &redSalvation,
          &plumBath,
          &sunset1,
          &candy,
          &greatBarrierReef,
          &peach,
          &denseWater,
          &sunset5,
          &salmonOnIce,
      });
  hyp->hub.subscribe(paletteColumn);
}
