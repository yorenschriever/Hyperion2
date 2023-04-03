#include "ChaserLedAnimation.h"
#include "ShapeChaserLedAnimation.h"
#include "LedShape.h"
#include "animationPattern.h"
#include "colours.h"
#include "core/distribution/inputs/inputSplitter.hpp"
#include "core/distribution/inputs/patternInput.hpp"
#include "core/distribution/inputs/udpInput.hpp"
#include "core/distribution/outputs/monitorOutput.hpp"
#include "core/distribution/outputs/neopixelOutput.hpp"
#include "core/distribution/pipes/convertPipe.hpp"
#include "core/generation/patterns/mappedPatterns.h"
#include "core/generation/pixelMap.hpp"
#include "core/generation/pixelMapSplitter.hpp"
#include "core/hyperion.hpp"
#include "gradient.hpp"
#include "ChaserLedAnimation.cpp"
#include "ShapeChaserLedAnimation.cpp"
#include "core/generation/patterns/helpers/tempo/constantTempo.h"

void addLedShapes(Hyperion *hyp);

Installation tetra = {
    LedShape(12, 3),  //  25cm Triangle ( 12 LEDs per side,  36 total,  2.16A powerdraw)
    LedShape(33, 3),  //  60cm Triangle ( 33 LEDs per side,  99 total,  5.94A powerdraw)
    LedShape(54, 3),  //  95cm Triangle ( 54 LEDs per side, 162 total,  9.72A powerdraw)
    LedShape(75, 3),  // 130cm Triangle ( 75 LEDs per side, 225 total, 13.50A powerdraw)
    LedShape(96, 3),  // 165cm Triangle ( 96 LEDs per side, 288 total, 17.28A powerdraw)
    LedShape(117, 3), // 200cm Triangle (117 LEDs per side, 351 total, 21.06A powerdraw)
};

Gradient heatmap = Gradient({
  {.position=   0, .color = RGB(    0,  0,  0)},  // Black
  {.position= 128, .color = RGB(  255,  0,  0)},  // Red
  {.position= 224, .color = RGB(  255,255,  0)},  // Bright yellow
  {.position= 255, .color = RGB(  255,255,255)}   // Full white
});

int main()
{
  auto hyp = new Hyperion();

  addLedShapes(hyp);

  Tempo::AddSource(new ConstantTempo(120));

  hyp->start();
  while (1) Thread::sleep(1000);
}

void addLedShapes(Hyperion *hyp)
{
  // This draws a 2d pixelMap from the installation configuration, so we can render it on the screen.
  // It only works for triangles now. if you need other shapes, it might be interesting to see if
  // the generation of the Installation and the PixelMap can be combined
  const float pixelPitch = 0.014;
  PixelMap tetraMap;
  for (LedShape shape : tetra)
  {
    for (int set = 0; set < shape.numSets; set++)
    {
      float x = ((shape.numLedsPerSet + 1) * pixelPitch) / -2.;
      float y = x * 0.59;
      float a = float(set) / 3. * 2 * M_PI;
      for (int led = 0; led < shape.numLedsPerSet; led++)
      {
        x += pixelPitch;
        tetraMap.push_back({.x = x * cos(a) + y * sin(a),
                            .y = y * cos(a) - x * sin(a)});
      }
    }
  }

  // Generate all pixel data in one go, and then split it up in six outputs,
  // because UDPOutput (and therefore MonitorOutput) are limited by a
  // maximum transfer size of 2*1440 bytes
  auto splitInput = new InputSplitter(
      new PatternInput<RGBA>(
          tetraMap.size(),
          // new Mapped::ConcentricWavePattern<SinFast>(tetraMap, 2, 2)),
          new AnimationPattern(&tetra, new ChaserLedAnimation(tetra, ChaserLedAnimation::relativeSize / 3, true), &heatmap)),
          //new AnimationPattern(&tetra, new ShapeChaserLedAnimation(true), &heatmap)),
      {12 * 3 * sizeof(RGBA),
       33 * 3 * sizeof(RGBA),
       54 * 3 * sizeof(RGBA),
       75 * 3 * sizeof(RGBA),
       96 * 3 * sizeof(RGBA),
       117 * 3 * sizeof(RGBA)},
      true);

  auto splitMap = PixelMapSplitter(
      &tetraMap, {12 * 3, 33 * 3, 54 * 3, 75 * 3, 96 * 3, 117 * 3});

  for (int i = 0; i < splitInput->size(); i++)
  {
    auto pipe = new ConvertPipe<RGBA, RGB>(
        splitInput->getInput(i),
        new MonitorOutput(splitMap.getMap(i)));
    hyp->addPipe(pipe);
  }
}