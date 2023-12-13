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
#include "movingheadPatterns.hpp"
#include "core/generation/palettes.hpp"
#include "patterns.hpp"
#include <vector>

#define COL_PALETTE 0
#define COL_MOVINGHEAD 1
#define COL_COLANDER 2
#define COL_FAIRYLIGHT_PINSPOT 3
#define COL_LASERS 4
#define COL_BULBS 5
#define COL_LEDSTER 6
#define COL_STROBES 7

void addColanderPipe(Hyperion *hyp);
void addLaserPipe(Hyperion *hyp);
void addBulbPipe(Hyperion *hyp);
void addFairylightPinspotPipe(Hyperion *hyp);
void addMovingHeadPipe(Hyperion *hyp);
void addPaletteColumn(Hyperion *hyp);

// LUT *PixelLut = new ColourCorrectionLUT(1.5, 255, 255, 255, 240);
LUT *LaserLut = new LaserLUT(0.5, 4096, 3048);
LUT *IncandescentLut = new IncandescentLUT(2.5, 4096, 200);
LUT *IncandescentLut8 = new IncandescentLUT(2.5, 255, 24);
LUT *GammaLut12 = new GammaLUT(2.5, 4096);
LUT *GammaLut8 = new GammaLUT(2.5, 255);

int main()
{
  auto hyp = new Hyperion();

  hyp->setMidiControllerFactory(new DinMidiControllerFactory<ApcMiniController>());

  addColanderPipe(hyp);
  addLaserPipe(hyp);
  addBulbPipe(hyp);
  addFairylightPinspotPipe(hyp);
  addMovingHeadPipe(hyp);

  addPaletteColumn(hyp);

  hyp->hub.setColumnName(COL_PALETTE, "Kleur");
  hyp->hub.setColumnName(COL_MOVINGHEAD, "Moving head");
  hyp->hub.setColumnName(COL_COLANDER, "Vergiet");
  hyp->hub.setColumnName(COL_FAIRYLIGHT_PINSPOT, "Fairylight / spiegelbol");
  hyp->hub.setColumnName(COL_LASERS, "Lasers");
  hyp->hub.setColumnName(COL_BULBS, "Peertjes");
  hyp->hub.setColumnName(COL_LEDSTER, "Ledster");
  hyp->hub.setColumnName(COL_STROBES, "Strobes");

  // Tempo::AddSource(new ConstantTempo(120));

  for (int i = 1; i < 7; i++)
    hyp->hub.setFlashColumn(i, false, true);
  hyp->hub.setFlashRow(5);
  hyp->hub.setFlashRow(6);
  hyp->hub.setFlashRow(7);

  hyp->hub.buttonPressed(COL_PALETTE, 0);
  hyp->hub.setForcedSelection(COL_PALETTE);
  hyp->hub.setFlashColumn(COL_PALETTE,false,true);

  hyp->start();

  while (1)
    Thread::sleep(1000);
}

void addColanderPipe(Hyperion *hyp)
{
  const int numColander = 10;
  auto input = new ControlHubInput<Monochrome>(
      numColander,
      &hyp->hub,
      {
          {.column = COL_COLANDER, .slot = 0, .pattern = new OnPattern()},
          {.column = COL_COLANDER, .slot = 1, .pattern = new GlowOriginalPattern()},
          {.column = COL_COLANDER, .slot = 2, .pattern = new SinPattern()},
          {.column = COL_COLANDER, .slot = 3, .pattern = new BeatSingleFadePattern()},
          {.column = COL_COLANDER, .slot = 4, .pattern = new BeatMultiFadePattern()},

          {.column = COL_COLANDER, .slot = 5, .pattern = new BlinderPattern()},
          {.column = COL_COLANDER, .slot = 6, .pattern = new SlowStrobePattern()},
          {.column = COL_COLANDER, .slot = 7, .pattern = new GlitchPattern()},

          {.column = COL_COLANDER, .slot = 8, .pattern = new FastStrobePattern()},
          {.column = COL_COLANDER, .slot = 9, .pattern = new BeatAllFadePattern()},
          {.column = COL_COLANDER, .slot = 10, .pattern = new BeatShakePattern()},
          {.column = COL_COLANDER, .slot = 11, .pattern = new BeatStepPattern()},
          {.column = COL_COLANDER, .slot = 12, .pattern = new GlowPattern()},
          {.column = COL_COLANDER, .slot = 13, .pattern = new FastStrobePattern2()},
      });

  auto splitInput = new InputSlicer(
      input,
      {
          {0, numColander, true},
          {0, numColander, true},
          {0, numColander, true},
      });

  hyp->addPipe(
      new ConvertPipe<Monochrome, Monochrome>(
          splitInput->getInput(0),
          new DMXOutput(128),
          IncandescentLut8));

  hyp->addPipe(
      new ConvertPipe<Monochrome, Monochrome12>(
          splitInput->getInput(1),
          new PWMOutput(1),
          IncandescentLut));

  hyp->addPipe(
      new ConvertPipe<Monochrome, Monochrome12>(
          splitInput->getInput(2),
          new UDPOutput("hyperslave6.local", 9620, 60),
          IncandescentLut));
}

void addLaserPipe(Hyperion *hyp)
{
  auto input = new ControlHubInput<Monochrome>(
      12,
      &hyp->hub,
      {
          {.column = COL_LASERS, .slot = 0, .pattern = new OnPattern(255)},
          {.column = COL_LASERS, .slot = 1, .pattern = new LFOPattern<PWM>()},
          {.column = COL_LASERS, .slot = 2, .pattern = new SinPattern()},
          {.column = COL_LASERS, .slot = 3, .pattern = new BeatMultiFadePattern()},
          {.column = COL_LASERS, .slot = 4, .pattern = new BeatShakePattern()},

          {.column = COL_LASERS, .slot = 5, .pattern = new BlinderPattern()},
          {.column = COL_LASERS, .slot = 6, .pattern = new FastStrobePattern()},
          {.column = COL_LASERS, .slot = 7, .pattern = new SlowStrobePattern()},

          {.column = COL_LASERS, .slot = 8, .pattern = new GlowPattern()},
          {.column = COL_LASERS, .slot = 9, .pattern = new BeatAllFadePattern()},
          {.column = COL_LASERS, .slot = 10, .pattern = new BeatSingleFadePattern()},
          {.column = COL_LASERS, .slot = 11, .pattern = new GlitchPattern()},
          {.column = COL_LASERS, .slot = 12, .pattern = new BeatStepPattern()},
          {.column = COL_LASERS, .slot = 13, .pattern = new FastStrobePattern2()},
      });

  auto pipe = new ConvertPipe<Monochrome, Monochrome12>(
      input,
      new UDPOutput("hyperslave7.local", 9620, 60),
      LaserLut);
  hyp->addPipe(pipe);
}

void addBulbPipe(Hyperion *hyp)
{
  const int numBulbs = 10;
  auto input = new ControlHubInput<Monochrome>(
      numBulbs,
      &hyp->hub,
      {
          {.column = COL_BULBS, .slot = 0, .pattern = new GlowPattern()},
          {.column = COL_BULBS, .slot = 1, .pattern = new SinPattern()},
          {.column = COL_BULBS, .slot = 2, .pattern = new LFOPattern<SawDown>()},
          {.column = COL_BULBS, .slot = 3, .pattern = new BeatShakePattern()},
          {.column = COL_BULBS, .slot = 4, .pattern = new BeatStepPattern()},

          {.column = COL_BULBS, .slot = 5, .pattern = new BlinderPattern()},
          {.column = COL_BULBS, .slot = 6, .pattern = new FastStrobePattern()},
          {.column = COL_BULBS, .slot = 7, .pattern = new GlitchPattern()},

          {.column = COL_BULBS, .slot = 8, .pattern = new SlowStrobePattern()},
          {.column = COL_BULBS, .slot = 9, .pattern = new BeatAllFadePattern()},
          {.column = COL_BULBS, .slot = 10, .pattern = new BeatSingleFadePattern()},
          {.column = COL_BULBS, .slot = 11, .pattern = new BeatMultiFadePattern()},
          {.column = COL_BULBS, .slot = 12, .pattern = new FastStrobePattern2()},
      });

  auto splitInput = new InputSlicer(
      input,
      {
          {0, numBulbs, true},
          {0, numBulbs, true},
      });

  hyp->addPipe(
      new ConvertPipe<Monochrome, Monochrome>(
          splitInput->getInput(0),
          new DMXOutput(256),
          GammaLut8));

  hyp->addPipe(
      new ConvertPipe<Monochrome, Monochrome12>(
          splitInput->getInput(1),
          new UDPOutput("hyperslave5.local", 9620, 60),
          GammaLut12));
}

void addFairylightPinspotPipe(Hyperion *hyp)
{
  hyp->addPipe(
      new ConvertPipe<Monochrome, Monochrome12>(
          new ControlHubInput<Monochrome>(
              1,
              &hyp->hub,
              {
                  {.column = COL_FAIRYLIGHT_PINSPOT, .slot = 0, .pattern = new OnPattern(255)},
                  {.column = COL_FAIRYLIGHT_PINSPOT, .slot = 1, .pattern = new SinPattern()},
                  {.column = COL_FAIRYLIGHT_PINSPOT, .slot = 2, .pattern = new BeatAllFadePattern()},
              }),
          new PWMOutput(11),
          IncandescentLut));

  hyp->addPipe(
      new ConvertPipe<Monochrome, Monochrome12>(
          new ControlHubInput<Monochrome>(
              1,
              &hyp->hub,
              {
                  {.column = COL_FAIRYLIGHT_PINSPOT, .slot = 3, .pattern = new OnPattern(255)},
                  {.column = COL_FAIRYLIGHT_PINSPOT, .slot = 4, .pattern = new SinPattern()},
                  {.column = COL_FAIRYLIGHT_PINSPOT, .slot = 5, .pattern = new BeatAllFadePattern()},
                  {.column = COL_FAIRYLIGHT_PINSPOT, .slot = 6, .pattern = new BlinderPattern()},
                  {.column = COL_FAIRYLIGHT_PINSPOT, .slot = 7, .pattern = new FastStrobePattern()},

              }),
          new PWMOutput(12),
          GammaLut12));
}

void addMovingHeadPipe(Hyperion *hyp)
{
  hyp->addPipe(
      new ConvertPipe<MovingHead, Miniwash7>(
          new ControlHubInput<MovingHead>(
              3,
              &hyp->hub,
              {
                  {.column = COL_MOVINGHEAD, .slot = 0, .pattern = new MovingheadPatterns::WallPattern()},
                  {.column = COL_MOVINGHEAD, .slot = 1, .pattern = new MovingheadPatterns::WallDJPattern()},
                  {.column = COL_MOVINGHEAD, .slot = 2, .pattern = new MovingheadPatterns::SidesPattern()},
                  {.column = COL_MOVINGHEAD, .slot = 3, .pattern = new MovingheadPatterns::Flash360Pattern()},
                  {.column = COL_MOVINGHEAD, .slot = 4, .pattern = new MovingheadPatterns::GlitchPattern()},
                  {.column = COL_MOVINGHEAD, .slot = 5, .pattern = new MovingheadPatterns::FrontPattern()},
                  {.column = COL_MOVINGHEAD, .slot = 6, .pattern = new MovingheadPatterns::UVPattern()},
                  {.column = COL_MOVINGHEAD, .slot = 7, .pattern = new MovingheadPatterns::GlitchPattern()},
              }),
          new DMXOutput(30)));
}

void addPaletteColumn(Hyperion *hyp)
{
  auto paletteColumn = new PaletteColumn(
      &hyp->hub,
      0,
      0,
      {
          &pinkSunset,
          &heatmap,
          &campfire,
          &retro,
          &sunset4,
          &tunnel,
          &blueOrange,
          &purpleGreen,

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
