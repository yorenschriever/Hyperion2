#include "colours.h"
#include "core/distribution/inputs/inputSplitter.hpp"
#include "core/distribution/inputs/patternInput.hpp"
#include "core/distribution/outputs/monitorOutput.hpp"
#include "core/distribution/outputs/monitorOutput3d.hpp"
#include "core/distribution/outputs/monitorOutput3dws.hpp"
#include "core/distribution/outputs/udpOutput.hpp"
#include "core/distribution/outputs/cloneOutput.hpp"
#include "core/distribution/pipes/convertPipe.hpp"
#include "core/generation/patterns/helpers/tempo/constantTempo.h"
#include "core/generation/pixelMap.hpp"
#include "core/generation/patterns/mappedPatterns.h"
#include "core/generation/pixelMapSplitter.hpp"
#include "core/generation/pixelMapSplitter3d.hpp"
#include "core/hyperion.hpp"
#include "distribution/inputs/controlHubInput.hpp"
#include "generation/controlHub/paletteColumn.hpp"
#include "mapping/columnMap.hpp"
#include "mapping/ledsterMap.hpp"
#include "mapping/columnMap3d.hpp"
#include "mapping/ledsterMap3d.hpp"
#include "mapping/haloMap3d.hpp"
#include "palettes.hpp"
#include "patterns.hpp"
#include "patterns3d.hpp"
#include "ledsterPatterns.hpp"
#include "platform/includes/thread.hpp"
#include "patterns-low.hpp"

#include "websocketServer.hpp"
#include "webServer.hpp"

#include <iostream>
#include <iterator>
#include <algorithm>

#include "webServerResponseBuilder.hpp"


auto pLedsterMap = ledsterMap.toPolarRotate90();
auto pColumnMap = columnMap.toPolarRotate90();

auto cColumnMap3d = columnMap3d.toCylindricalRotate90();

void addLedsterPipe(Hyperion *hyp);
void addColumnPipes(Hyperion *hyp);
void addHaloPipe(Hyperion *hyp);
void addPaletteColumn(Hyperion *hyp);


auto serv = WebServer::createInstance();


int main()
{
  auto hyp = new Hyperion();

  hyp->hub.params.primaryColour = RGB(255, 0, 255);
  hyp->hub.params.secondaryColour = RGB(50, 50, 50);
  hyp->hub.params.highlightColour = RGB(250, 250, 250);
  hyp->hub.params.palette = &heatmap;

  addColumnPipes(hyp);
  addLedsterPipe(hyp);
  //addHaloPipe(hyp);
  addPaletteColumn(hyp);

  Tempo::AddSource(ConstantTempo::getInstance());
  ConstantTempo::getInstance()->setBpm(120);

  hyp->hub.buttonPressed(0,0);

  hyp->start();
  while (1)
    Thread::sleep(1000);
}


// auto ledsterPattern = new FWF::RibbenFasePattern();
// auto columnPattern = new FWF::RibbenFasePattern();

// auto ledsterPattern = new FWF::SegmentChasePattern();
// auto columnPattern = new FWF::SegmentChasePattern();

// auto ledsterPattern = new FWF::SegmentChasePattern(); //X
// auto columnPattern = new FWF::OnBeatColumnFadePattern();

// auto ledsterPattern = new FWF3D::OnBeatColumnChaseUpPattern(ledsterMap3d); //X
// auto columnPattern = new FWF3D::OnBeatColumnChaseUpPattern(columnMap3d);

// auto ledsterPattern = new FWF3D::GrowingCirclesPattern(ledsterMap3d); //X
// auto columnPattern = new FWF3D::GrowingCirclesPattern(columnMap3d);

// auto ledsterPattern = new FWF3D::LineLaunch(ledsterMap3d); //X
// auto columnPattern = new FWF3D::LineLaunch(columnMap3d);


//auto columnPattern = new Low::StaticGradientPattern(columnMap3d);


void addLedsterPipe(Hyperion *hyp)
{
  auto ledsterPipe = new ConvertPipe<RGBA, RGB>(
      new ControlHubInput<RGBA>(
          ledsterMap.size(),
          &hyp->hub,
          {
              // {.column = 0, .slot = 0, .pattern = new FWF3D::RadialGlitterFadePattern(ledsterMap3d)},
              // {.column = 0, .slot = 1, .pattern = new FWF3D::AngularFadePattern(ledsterMap3d)},
              // {.column = 0, .slot = 2, .pattern = new FWF::GlowPulsePattern()},

              // {.column = 1, .slot = 0, .pattern = new FWF::SquareGlitchPattern(ledsterMap)},
              // {.column = 1, .slot = 1, .pattern = new FWF::GrowingStrobePattern(pLedsterMap)},
              // {.column = 1, .slot = 2, .pattern = new Ledster::RadialFadePattern(ledsterMap)},

              // {.column = 2, .slot = 0, .pattern = new Ledster::ClivePattern<SinFast>(10000)},
              // {.column = 2, .slot = 1, .pattern = new Ledster::RibbenClivePattern<LFOPause<NegativeCosFast>>(10000,1,0.25)},
              // {.column = 2, .slot = 2, .pattern = new Ledster::SnakePattern()},

              // {.column = 3, .slot = 0, .pattern = new Ledster::RibbenClivePattern<SoftSquare>(40000)},
              // {.column = 3, .slot = 1, .pattern = new Ledster::RibbenClivePattern<SawDown>(500)},
              // {.column = 3, .slot = 2, .pattern = new Ledster::RibbenClivePattern<SinFast>(3000)},

              // {.column = 4, .slot = 0, .pattern = new Ledster::RibbenClivePattern<LFOPause<NegativeCosFast>>(10000,1,0.15)},
              // {.column = 4, .slot = 1, .pattern = new Ledster::RibbenClivePattern<LFOPause<SawDown>>(10000,1,0.15)},
              // {.column = 4, .slot = 2, .pattern = new Ledster::RibbenClivePattern<PWM>(10000,1,0.0025)},

              // {.column = 5, .slot = 0, .pattern = new FWF::RibbenFlashPattern()},
               {.column = 0, .slot = 0, .pattern = new Ledster::ChevronsPattern(ledsterMap)},
              // {.column = 5, .slot = 2, .pattern = new FWF3D::ChevronsConePattern(ledsterMap3d)},
              // //{.column = 5, .slot = 2, .pattern = new Ledster::PixelGlitchPattern()},

              // {.column = 6, .slot = 0, .pattern = new FWF::RibbenFadePattern()},
              // {.column = 6, .slot = 1, .pattern = new FWF::SegmentChasePattern()},
              // //{.column = 6, .slot = 2, .pattern = new FWF::OnBeatColumnFadePattern()},

              // //{.column = 7, .slot = 0, .pattern = new FWF3D::OnBeatColumnChaseUpPattern(ledsterMap3d)},
              // //{.column = 7, .slot = 1, .pattern = new FWF3D::GrowingCirclesPattern(ledsterMap3d)},
              // //{.column = 7, .slot = 2, .pattern = new FWF3D::LineLaunch(ledsterMap3d)},
          }),
      //new PatternInput<RGBA>(ledsterMap3d.size(), ledsterPattern),
      new CloneOutput({
        new MonitorOutput3dws(ledsterMap3d,serv),
        new UDPOutput("hyperslave1.local",9611,60)
      }));
  hyp->addPipe(ledsterPipe);
}

void addColumnPipes(Hyperion *hyp)
{
  // Generate 1 pattern, and split it up in six outputs,
  // because UDPOutput (and therefore MonitorOutput) are limited by a
  // maximum transfer size of 2*1440 bytes
  auto splitInput = new InputSplitter(
      new ControlHubInput<RGBA>(
          columnMap.size(),
          &hyp->hub,
          {
              // {.column = 0, .slot = 0, .pattern = new FWF3D::RadialGlitterFadePattern(columnMap3d)},
              // {.column = 0, .slot = 1, .pattern = new FWF3D::AngularFadePattern(columnMap3d)},
              // {.column = 0, .slot = 2, .pattern = new FWF::GlowPulsePattern()},

              // {.column = 1, .slot = 0, .pattern = new FWF::SquareGlitchPattern(columnMap)},
              // {.column = 1, .slot = 1, .pattern = new FWF::GrowingStrobePattern(pColumnMap)},
              // {.column = 1, .slot = 2, .pattern = new Mapped::HorizontalGradientPattern(columnMap)},

              // {.column = 2, .slot = 0, .pattern = new Mapped::HorizontalGradientPattern(columnMap)},
              // {.column = 2, .slot = 1, .pattern = new FWF::RibbenClivePattern<LFOPause<NegativeCosFast>>(10000,1,0.25)},
              // {.column = 2, .slot = 2, .pattern = new Mapped::HorizontalGradientPattern(columnMap)},

              // {.column = 3, .slot = 0, .pattern = new FWF::RibbenClivePattern<SoftSquare>(40000)},
              // {.column = 3, .slot = 1, .pattern = new FWF::RibbenClivePattern<SawDown>(500)},
              // {.column = 3, .slot = 2, .pattern = new FWF::RibbenClivePattern<SinFast>(3000)},

              // {.column = 4, .slot = 0, .pattern = new FWF::RibbenClivePattern<LFOPause<NegativeCosFast>>(10000,1,0.15)},
              // {.column = 4, .slot = 1, .pattern = new FWF::RibbenClivePattern<LFOPause<SawDown>>(10000,1,0.15)},
              // {.column = 4, .slot = 2, .pattern = new FWF::RibbenClivePattern<PWM>(10000,1,0.0025)},

              // {.column = 5, .slot = 0, .pattern = new FWF::RibbenFlashPattern()},
               {.column = 0, .slot = 0, .pattern = new Ledster::ChevronsPattern(columnMap)},
              // {.column = 5, .slot = 2, .pattern = new FWF3D::ChevronsConePattern(columnMap3d)},
              // // {.column = 5, .slot = 2, .pattern = new Ledster::PixelGlitchPattern()},

              // {.column = 6, .slot = 0, .pattern = new FWF::RibbenFadePattern()},
              // {.column = 6, .slot = 1, .pattern = new FWF::SegmentChasePattern()},
              // {.column = 6, .slot = 2, .pattern = new FWF::OnBeatColumnFadePattern()},

              // {.column = 7, .slot = 0, .pattern = new FWF3D::OnBeatColumnChaseUpPattern(columnMap3d)},
              // {.column = 7, .slot = 1, .pattern = new FWF3D::GrowingCirclesPattern(columnMap3d)},
              // {.column = 7, .slot = 2, .pattern = new FWF3D::LineLaunch(columnMap3d)},

              //{.column = 0, .slot = 0, .pattern = new Low::StaticGradientPattern(columnMap3d)},
              // {.column = 0, .slot = 0, .pattern = new Low::OnBeatColumnChaseUpPattern(columnMap3d)},
              // {.column = 0, .slot = 0, .pattern = new Low::HorizontalSin(cColumnMap3d)},
              // {.column = 0, .slot = 0, .pattern = new Low::HorizontalSaw(cColumnMap3d)},
              // {.column = 0, .slot = 0, .pattern = new Low::GrowShrink(cColumnMap3d)},
              // {.column = 0, .slot = 0, .pattern = new Low::GlowPulsePattern(columnMap3d)},
              //{.column = 0, .slot = 0, .pattern = new Low::VerticallyIsolated(cColumnMap3d)},

              //{.column=0, .slot=0, .pattern=new TestPattern()}
          }),
    //new PatternInput<RGBA>(columnMap3d.size(), columnPattern),
      {480 * sizeof(RGBA),
       480 * sizeof(RGBA),
       480 * sizeof(RGBA),
       480 * sizeof(RGBA),
       480 * sizeof(RGBA),
       480 * sizeof(RGBA)},
      true);

  auto splitMap = PixelMapSplitter3d(
      &columnMap3d, {480, 480, 480, 480, 480, 480});

  for (int i = 0; i < splitInput->size(); i++)
  {
    auto pipe = new ConvertPipe<RGBA, RGB>(
        splitInput->getInput(i),
        new MonitorOutput3dws(splitMap.getMap(i),serv));
        //new MonitorOutput3d(splitMap.getMap(i)));
    hyp->addPipe(pipe);
  }
}

void addHaloPipe(Hyperion *hyp)
{
  auto haloPipe = new ConvertPipe<RGBA, RGB>(
      new ControlHubInput<RGBA>(
          ledsterMap.size(),
          &hyp->hub,
          {
              {.column = 0, .slot = 0, .pattern = new FWF3D::RadialGlitterFadePattern(haloMap3d)},
              {.column = 0, .slot = 1, .pattern = new FWF3D::AngularFadePattern(haloMap3d)},
              {.column = 0, .slot = 2, .pattern = new FWF::GlowPulsePattern()},

              //{.column = 1, .slot = 0, .pattern = new FWF::SquareGlitchPattern(haloMap3d)},
              //{.column = 1, .slot = 1, .pattern = new FWF::GrowingStrobePattern(haloMap3d)},
              //{.column = 1, .slot = 2, .pattern = new Ledster::RadialFadePattern(haloMap3d)},

              {.column = 2, .slot = 0, .pattern = new Ledster::ClivePattern<SinFast>(10000)},
              {.column = 2, .slot = 1, .pattern = new Ledster::RibbenClivePattern<LFOPause<NegativeCosFast>>(10000,1,0.25)},
              {.column = 2, .slot = 2, .pattern = new Ledster::SnakePattern()},

              {.column = 3, .slot = 0, .pattern = new Ledster::RibbenClivePattern<SoftSquare>(40000)},
              {.column = 3, .slot = 1, .pattern = new Ledster::RibbenClivePattern<SawDown>(500)},
              {.column = 3, .slot = 2, .pattern = new Ledster::RibbenClivePattern<SinFast>(3000)},

              {.column = 4, .slot = 0, .pattern = new Ledster::RibbenClivePattern<LFOPause<NegativeCosFast>>(10000,1,0.15)},
              {.column = 4, .slot = 1, .pattern = new Ledster::RibbenClivePattern<LFOPause<SawDown>>(10000,1,0.15)},
              {.column = 4, .slot = 2, .pattern = new Ledster::RibbenClivePattern<PWM>(10000,1,0.0025)},

              {.column = 5, .slot = 0, .pattern = new FWF::RibbenFlashPattern()},
              //{.column = 5, .slot = 1, .pattern = new Ledster::ChevronsPattern(ledsterMap)},
              {.column = 5, .slot = 2, .pattern = new FWF3D::ChevronsConePattern(haloMap3d)},
              //{.column = 5, .slot = 2, .pattern = new Ledster::PixelGlitchPattern()},

              {.column = 6, .slot = 0, .pattern = new FWF::RibbenFadePattern()},
              {.column = 6, .slot = 1, .pattern = new FWF::SegmentChasePattern()},
              //{.column = 6, .slot = 2, .pattern = new FWF::OnBeatColumnFadePattern()},

              //{.column = 7, .slot = 0, .pattern = new FWF3D::OnBeatColumnChaseUpPattern(ledsterMap3d)},
              //{.column = 7, .slot = 1, .pattern = new FWF3D::GrowingCirclesPattern(ledsterMap3d)},
              //{.column = 7, .slot = 2, .pattern = new FWF3D::LineLaunch(ledsterMap3d)},
          }),
      new MonitorOutput3dws(haloMap3d,serv));
  hyp->addPipe(haloPipe);
}

void addPaletteColumn(Hyperion *hyp){
  auto paletteColumn = new PaletteColumn(&hyp->hub,0, {
    &heatmap,
    &sunset1,
    &sunset3,
    &sunset4
  });
  hyp->hub.subscribe(paletteColumn);
}