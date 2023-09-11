#include "ChaserLedAnimation.h"
#include "LedShape.h"
#include "ShapeChaserLedAnimation.h"
#include "ShapeChaserLedAnimation.h"
#include "animationPattern.h"
#include "colours.h"
#include "core/distribution/inputs/inputSplitter.hpp"
#include "core/distribution/inputs/patternInput.hpp"
#include "core/distribution/inputs/udpInput.hpp"
#include "core/distribution/outputs/monitorOutput.hpp"
#include "core/distribution/outputs/neopixelOutput.hpp"
#include "core/distribution/pipes/convertPipe.hpp"
#include "core/generation/patterns/helpers/tempo/constantTempo.h"
#include "core/generation/patterns/mappedPatterns.h"
#include "core/generation/pixelMap.hpp"
#include "core/hyperion.hpp"
#include "gradient.hpp"

// TODO why doesn't cmake pick these up automatically?
#include "ChaserLedAnimation.cpp"
#include "ShapeChaserLedAnimation.cpp"

void addLedShapes(Hyperion *hyp);
void addPipe(Hyperion *hyp);

PixelMap tetraMap;

Installation tetra = {
    LedShape(12, 3),  //  25cm Triangle ( 12 LEDs per side,  36 total,  2.16A powerdraw)
    LedShape(33, 3),  //  60cm Triangle ( 33 LEDs per side,  99 total,  5.94A powerdraw)
    LedShape(54, 3),  //  95cm Triangle ( 54 LEDs per side, 162 total,  9.72A powerdraw)
    LedShape(75, 3),  // 130cm Triangle ( 75 LEDs per side, 225 total, 13.50A powerdraw)
    LedShape(96, 3),  // 165cm Triangle ( 96 LEDs per side, 288 total, 17.28A powerdraw)
    LedShape(117, 3), // 200cm Triangle (117 LEDs per side, 351 total, 21.06A powerdraw)
};

Gradient heatmap = Gradient({
    {.position = 0, .color = RGB(0, 0, 0)},        // Black
    {.position = 128, .color = RGB(255, 0, 0)},    // Red
    {.position = 224, .color = RGB(255, 255, 0)},  // Bright yellow
    {.position = 255, .color = RGB(255, 255, 255)} // Full white
});

int main()
{
  auto hyp = new Hyperion();

  hyp->hub.getParams(0)->name = "Led Chaser CW";
  hyp->hub.addParams(new Params("Led Chaser CCW"));
  hyp->hub.addParams(new Params("Shape Chaser Out"));
  hyp->hub.addParams(new Params("Shape Chaser In"));

  hyp->hub.setColumnName(0, "Led Chaser CW");
  hyp->hub.setColumnName(1, "Led Chaser CCW");
  hyp->hub.setColumnName(2, "Shape Chaser Out");
  hyp->hub.setColumnName(3, "Shape Chaser In");

  hyp->hub.getParams(0)->gradient = &heatmap;
  addLedShapes(hyp);
  addPipe(hyp);

  Tempo::AddSource(new ConstantTempo(120));

  hyp->hub.buttonPressed(0,2);

  hyp->start();
  while (1)
    Thread::sleep(1000);
}

void addLedShapes(Hyperion *hyp)
{
  // This draws a 2d pixelMap from the installation configuration, so we can render it on the screen.
  // It only works for triangles now. if you need other shapes, it might be interesting to see if
  // the generation of the Installation and the PixelMap can be combined
  const float pixelPitch = 0.014;
  for (LedShape shape : tetra)
  {
    float a =0;
    float x = ((shape.numLedsPerSet + 1) * pixelPitch) / -2.;
    float y = x * 0.59;
    for (int set = 0; set < shape.numSets; set++)
    {
      for (int led = 0; led < shape.numLedsPerSet; led++)
      {
        tetraMap.push_back({.x = x, .y = y });
        x += pixelPitch * cos(a);
        y += pixelPitch * sin(a);
      }
      a += 120./360. * 2 * M_PI;
    }
  }

}

void addPipe(Hyperion *hyp)
{
  auto input = new ControlHubInput<RGBA>(
    tetraMap.size(),
    &hyp->hub,
    {
      {.column = 0, .slot = 0, .paramsSlot=0, .pattern=new AnimationPattern(&tetra, new ChaserLedAnimation(tetra, ChaserLedAnimation::relativeSize / 3, true), "Led Chaser CW")},
      {.column = 1, .slot = 0, .paramsSlot=1, .pattern=new AnimationPattern(&tetra, new ChaserLedAnimation(tetra, ChaserLedAnimation::relativeSize / 3, false), "Led Chaser CCW")},
      {.column = 2, .slot = 0, .paramsSlot=2, .pattern=new AnimationPattern(&tetra, new ShapeChaserLedAnimation(tetra, true), "Shape Chaser CW")},
      {.column = 3, .slot = 0, .paramsSlot=3, .pattern=new AnimationPattern(&tetra, new ShapeChaserLedAnimation(tetra, false), "Shape Chaser CCW")}
    }
  );

#if ESP_PLATFORM
  // Generate all pixel data in one go, and then split it up in six outputs,
  // because UDPOutput (and therefore MonitorOutput) are limited by a
  // maximum transfer size of 2*1440 bytes
  auto splitInput = new InputSplitter(
      input,
      {12 * 3 * sizeof(RGBA),
       33 * 3 * sizeof(RGBA),
       54 * 3 * sizeof(RGBA),
       75 * 3 * sizeof(RGBA),
       96 * 3 * sizeof(RGBA),
       117 * 3 * sizeof(RGBA)},
      true);

  for (int i = 0; i < splitInput->size(); i++)
  {
    hyp->addPipe(
        new ConvertPipe<RGBA, RGB>(
            splitInput->getInput(i),
            new NeopixelOutput(i)));
  }
#else

  hyp->addPipe(new ConvertPipe<RGBA, RGB>(
      input,
      new MonitorOutput(&hyp->webServer, &tetraMap)));

#endif
}