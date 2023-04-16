#include "colours.h"
#include "core/distribution/inputs/inputSlicer.hpp"
#include "core/distribution/inputs/inputSplitter.hpp"
#include "core/distribution/inputs/patternInput.hpp"
#include "core/distribution/outputs/cloneOutput.hpp"
#include "core/distribution/outputs/monitorOutput.hpp"
#include "core/distribution/outputs/monitorOutput3d.hpp"
#include "core/distribution/outputs/monitorOutput3dws.hpp"
#include "core/distribution/outputs/udpOutput.hpp"
#include "core/distribution/pipes/convertPipe.hpp"
#include "core/generation/patterns/helpers/tempo/constantTempo.h"
#include "core/generation/patterns/mappedPatterns.h"
#include "core/generation/pixelMap.hpp"
#include "core/generation/pixelMapSplitter.hpp"
#include "core/generation/pixelMapSplitter3d.hpp"
#include "core/hyperion.hpp"
#include "distribution/inputs/controlHubInput.hpp"
#include "generation/controlHub/paletteColumn.hpp"
#include "generation/controlHub/websocketController.hpp"
#include "ledsterPatterns.hpp"
#include "mapping/columnMap.hpp"
#include "mapping/columnMap3d.hpp"
#include "mapping/haloMap3d.hpp"
#include "mapping/ledsterMap.hpp"
#include "mapping/ledsterMap3d.hpp"
#include "palettes.hpp"
#include "patterns-hi.hpp"
#include "patterns-low.hpp"
#include "patterns-max.hpp"
#include "patterns-mid.hpp"
#include "patterns-min.hpp"
#include "patterns-test.hpp"
#include "patterns.hpp"
#include "patterns3d.hpp"
#include "platform/includes/thread.hpp"
#include "webServer.hpp"
#include "webServerResponseBuilder.hpp"
#include "websocketServer.hpp"
#include <algorithm>
#include <iostream>
#include <iterator>

auto pLedsterMap = ledsterMap.toPolarRotate90();
auto pColumnMap = columnMap.toPolarRotate90();

auto cColumnMap3d = columnMap3d.toCylindricalRotate90();
auto cLedsterMap3d = ledsterMap3d.toCylindricalRotate90();
auto cHaloMap3d = haloMap3d.toCylindricalRotate90();

void addLedsterPipe(Hyperion *hyp);
void addColumnPipes(Hyperion *hyp);
void addHaloPipe(Hyperion *hyp);
void addPaletteColumn(Hyperion *hyp);

auto serv = WebServer::createInstance();

int main()
{
  auto hyp = new Hyperion();

  addColumnPipes(hyp);
  addLedsterPipe(hyp);
  addHaloPipe(hyp);
  addPaletteColumn(hyp);

  hyp->hub.subscribe(new WebsocketController(&hyp->hub));

  Tempo::AddSource(new ConstantTempo(120));

  // select first palette
  hyp->hub.buttonPressed(0, 0);

  hyp->start();
  while (1)
    Thread::sleep(1000);
}

void addLedsterPipe(Hyperion *hyp)
{
  auto ledsterPipe = new ConvertPipe<RGBA, RGB>(
      new ControlHubInput<RGBA>(
          ledsterMap.size(),
          &hyp->hub,
          {
              {.column = 8, .slot = 0, .pattern = new TestPatterns::ShowStarts(271)},

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
              // {.column = 0, .slot = 0, .pattern = new Ledster::ChevronsPattern(ledsterMap)},
              // {.column = 5, .slot = 2, .pattern = new FWF3D::ChevronsConePattern(ledsterMap3d)},
              // //{.column = 5, .slot = 2, .pattern = new Ledster::PixelGlitchPattern()},

              // {.column = 6, .slot = 0, .pattern = new FWF::RibbenFadePattern()},
              // {.column = 6, .slot = 1, .pattern = new FWF::SegmentChasePattern()},
              // //{.column = 6, .slot = 2, .pattern = new FWF::OnBeatColumnFadePattern()},

              // //{.column = 7, .slot = 0, .pattern = new FWF3D::OnBeatColumnChaseUpPattern(ledsterMap3d)},
              // //{.column = 7, .slot = 1, .pattern = new FWF3D::GrowingCirclesPattern(ledsterMap3d)},
              // //{.column = 7, .slot = 2, .pattern = new FWF3D::LineLaunch(ledsterMap3d)},

              {.column = 2, .slot = 0, .pattern = new Mid::Lighthouse(cLedsterMap3d)},
              {.column = 2, .slot = 2, .pattern = new Mid::Halo2(cLedsterMap3d)},
              {.column = 2, .slot = 3, .pattern = new Mid::HaloOnBeat(cLedsterMap3d)},
              {.column = 2, .slot = 4, .pattern = new Mid::SnowflakePatternLedster()},
              {.column = 2, .slot = 6, .pattern = new Mid::PetalChase(cLedsterMap3d)},

              {.column = 3, .slot = 0, .pattern = new Hi::SnakePattern()}, // TODO generate pattern
              {.column = 3, .slot = 1, .pattern = new Hi::XY(ledsterMap3d)},
              {.column = 3, .slot = 2, .pattern = new Hi::PetalRotatePattern()},
              {.column = 3, .slot = 3, .pattern = new Hi::HexBeatPattern()},

              // {.column = 4, .slot = 0, .pattern = new Min::RibbenClivePattern<LFOPause<NegativeCosFast>>(10000,1,0.25)},
              // {.column = 4, .slot = 1, .pattern = new Min::RibbenClivePattern<SoftSquare>(40000)},
              // {.column = 4, .slot = 2, .pattern = new Min::RibbenClivePattern<SawDown>(500)},
              // {.column = 4, .slot = 3, .pattern = new Min::RibbenClivePattern<SinFast>(3000)},

              // {.column = 4, .slot = 5, .pattern = new Min::RibbenClivePattern<LFOPause<SawDown>>(10000,1,0.15)},
              // {.column = 4, .slot = 6, .pattern = new Min::RibbenClivePattern<PWM>(10000,1,0.0025)},

              // TODO vanaf hier nog parameters en kleuren checken
              {.column = 4, .slot = 0, .pattern = new Min::RibbenClivePattern<LFOPause<NegativeCosFast>>(10000, 1, 0.15)},
              {.column = 4, .slot = 1, .pattern = new Min::RibbenFlashPattern()},
              {.column = 4, .slot = 2, .pattern = new Min::SegmentChasePattern()},
              {.column = 4, .slot = 3, .pattern = new Min::LineLaunch(ledsterMap3d)},
              {.column = 4, .slot = 4, .pattern = new Min::GrowingCirclesPattern(ledsterMap3d)},
              {.column = 4, .slot = 5, .pattern = new Min::GlowPulsePattern()},
              //{.column = 4, .slot = 5, .pattern = new FWF3D::OnBeatColumnChaseUpPattern(ledsterMap3d)},

              {.column = 5, .slot = 0, .pattern = new Max::RadialGlitterFadePattern(ledsterMap3d)},
              {.column = 5, .slot = 1, .pattern = new Max::AngularFadePattern(ledsterMap3d)},
              {.column = 5, .slot = 2, .pattern = new Max::GrowingStrobePattern(cLedsterMap3d)},
              {.column = 5, .slot = 3, .pattern = new Max::RadialFadePattern(cLedsterMap3d)},
              {.column = 5, .slot = 4, .pattern = new Max::ChevronsPattern(ledsterMap3d)},
              {.column = 5, .slot = 5, .pattern = new Max::ChevronsConePattern(ledsterMap3d)},

          }),

      new CloneOutput({new MonitorOutput3dws(ledsterMap3d, serv),
                       // new MonitorOutput3d(ledsterMap3d),
                       new UDPOutput("ledster.local", 9611, 60)}));
  hyp->addPipe(ledsterPipe);
}

void addColumnPipes(Hyperion *hyp)
{
  // Generate 1 pattern, and split it up in six outputs,
  // because UDPOutput (and therefore MonitorOutput) are limited by a
  // maximum transfer size of 2*1440 bytes

  auto columnsInput = new ControlHubInput<RGBA>(
      columnMap.size(),
      &hyp->hub,
      {
          {.column = 8, .slot = 0, .pattern = new TestPatterns::ShowStarts(60)},

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
          // {.column = 0, .slot = 0, .pattern = new Ledster::ChevronsPattern(columnMap)},
          // {.column = 5, .slot = 2, .pattern = new FWF3D::ChevronsConePattern(columnMap3d)},
          // // {.column = 5, .slot = 2, .pattern = new Ledster::PixelGlitchPattern()},

          // {.column = 6, .slot = 0, .pattern = new FWF::RibbenFadePattern()},
          // {.column = 6, .slot = 1, .pattern = new FWF::SegmentChasePattern()},
          // {.column = 6, .slot = 2, .pattern = new FWF::OnBeatColumnFadePattern()},

          // {.column = 7, .slot = 0, .pattern = new FWF3D::OnBeatColumnChaseUpPattern(columnMap3d)},
          // {.column = 7, .slot = 1, .pattern = new FWF3D::GrowingCirclesPattern(columnMap3d)},
          // {.column = 7, .slot = 2, .pattern = new FWF3D::LineLaunch(columnMap3d)},

          {.column = 1, .slot = 0, .pattern = new Low::StaticGradientPattern(columnMap3d)},
          {.column = 1, .slot = 1, .pattern = new Low::OnBeatColumnChaseUpPattern(columnMap3d)},
          {.column = 1, .slot = 2, .pattern = new Low::HorizontalSin(cColumnMap3d)},
          {.column = 1, .slot = 3, .pattern = new Low::HorizontalSaw(cColumnMap3d)},
          {.column = 1, .slot = 4, .pattern = new Low::GrowShrink(cColumnMap3d)},
          {.column = 1, .slot = 5, .pattern = new Low::GlowPulsePattern(columnMap3d)},
          {.column = 1, .slot = 6, .pattern = new Low::VerticallyIsolated(cColumnMap3d)},

          {.column = 2, .slot = 0, .pattern = new Mid::Lighthouse(cColumnMap3d)},
          {.column = 2, .slot = 1, .pattern = new Mid::Halo(cColumnMap3d)},
          {.column = 2, .slot = 3, .pattern = new Mid::HaloOnBeat(cColumnMap3d)},
          {.column = 2, .slot = 4, .pattern = new Mid::SnowflakePatternColumn(cColumnMap3d)},
          {.column = 2, .slot = 5, .pattern = new Mid::TakkenChase(cColumnMap3d)},
          // {.column = 2, .slot = 6, .pattern = new Mid::HaloChase(cColumnMap3d)},

          {.column = 3, .slot = 1, .pattern = new Hi::XY(columnMap3d)},

          {.column = 4, .slot = 0, .pattern = new Min::RibbenClivePattern<LFOPause<NegativeCosFast>>(10000, 1, 0.15)},
          {.column = 4, .slot = 1, .pattern = new Min::RibbenFlashPattern()},
          {.column = 4, .slot = 2, .pattern = new Min::SegmentChasePattern()},
          {.column = 4, .slot = 3, .pattern = new Min::LineLaunch(columnMap3d)},
          {.column = 4, .slot = 4, .pattern = new Min::GrowingCirclesPattern(columnMap3d)},
          {.column = 4, .slot = 5, .pattern = new Min::GlowPulsePattern()},
          //{.column = 4, .slot = 5, .pattern = new FWF3D::OnBeatColumnChaseUpPattern(columnMap3d)},

          {.column = 5, .slot = 0, .pattern = new Max::RadialGlitterFadePattern(columnMap3d)},
          {.column = 5, .slot = 1, .pattern = new Max::AngularFadePattern(columnMap3d)},
          {.column = 5, .slot = 2, .pattern = new Max::GrowingStrobePattern(cColumnMap3d)},
          {.column = 5, .slot = 3, .pattern = new Max::RadialFadePattern(cColumnMap3d)},
          {.column = 5, .slot = 4, .pattern = new Max::ChevronsPattern(columnMap3d)},
          {.column = 5, .slot = 5, .pattern = new Max::ChevronsConePattern(columnMap3d)},

      });

  auto splitInput = new InputSlicer(
      columnsInput,
      // new PatternInput<RGBA>(columnMap3d.size(), columnPattern),
      {
          {0 * sizeof(RGBA), 360 * sizeof(RGBA)},
          {360 * sizeof(RGBA), 120 * sizeof(RGBA)},
          {480 * sizeof(RGBA), 360 * sizeof(RGBA)},
          {840 * sizeof(RGBA), 120 * sizeof(RGBA)},
          {960 * sizeof(RGBA), 360 * sizeof(RGBA)},
          {1320 * sizeof(RGBA), 120 * sizeof(RGBA)},
          {1440 * sizeof(RGBA), 360 * sizeof(RGBA)},
          {1800 * sizeof(RGBA), 120 * sizeof(RGBA)},
          {1920 * sizeof(RGBA), 360 * sizeof(RGBA)},
          {2280 * sizeof(RGBA), 120 * sizeof(RGBA)},
          {2400 * sizeof(RGBA), 360 * sizeof(RGBA)},
          {2760 * sizeof(RGBA), 120 * sizeof(RGBA)},
          {0, 6 * 8 * 60 * sizeof(RGBA)}},
      true);

  auto splitMap = PixelMapSplitter3d(
      &columnMap3d, {
                        360,
                        120,
                        360,
                        120,
                        360,
                        120,
                        360,
                        120,
                        360,
                        120,
                        360,
                        120,
                    });

  typedef struct
  {
    const char *host;
    const unsigned short port;
  } Slave;
  Slave slaves[] = {
      {.host = "hyperslave1.local", .port = 9611}, // rode kolom
      {.host = "hyperslave1.local", .port = 9612}, // rode punt
      {.host = "hyperslave1.local", .port = 9613}, // groene kolom
      {.host = "hyperslave1.local", .port = 9614}, // groene punt
      {.host = "hyperslave2.local", .port = 9611}, // blauwe kolom
      {.host = "hyperslave2.local", .port = 9612}, // blauwe punt
      {.host = "hyperslave2.local", .port = 9613}, // azuur kolom
      {.host = "hyperslave2.local", .port = 9614}, // azuur punt
      {.host = "hyperslave3.local", .port = 9611}, // paars kolom
      {.host = "hyperslave3.local", .port = 9612}, // paars punt
      {.host = "hyperslave3.local", .port = 9613}, // geel kolom
      {.host = "hyperslave3.local", .port = 9614}, // geel punt
  };

  for (int i = 0; i < splitInput->size() - 1; i++)
  {
    auto pipe = new ConvertPipe<RGBA, RGB>(
        splitInput->getInput(i),
        // new CloneOutput({
        // new MonitorOutput3dws(splitMap.getMap(i), serv),
        // new MonitorOutput3d(splitMap.getMap(i)),
        new UDPOutput(slaves[i].host, slaves[i].port, 60)
        //})
    );
    hyp->addPipe(pipe);
  }

  hyp->addPipe(
      new ConvertPipe<RGBA, RGB>(
          splitInput->getInput(12),
          new MonitorOutput3dws(columnMap3d, serv)));
}

void addHaloPipe(Hyperion *hyp)
{
  auto haloPipe = new ConvertPipe<RGBA, RGB>(
      new ControlHubInput<RGBA>(
          ledsterMap.size(),
          &hyp->hub,
          {
              {.column = 8, .slot = 0, .pattern = new TestPatterns::ShowStarts(100)},

              // {.column = 0, .slot = 0, .pattern = new FWF3D::RadialGlitterFadePattern(haloMap3d)},
              // {.column = 0, .slot = 1, .pattern = new FWF3D::AngularFadePattern(haloMap3d)},
              // {.column = 0, .slot = 2, .pattern = new FWF::GlowPulsePattern()},

              // //{.column = 1, .slot = 0, .pattern = new FWF::SquareGlitchPattern(haloMap3d)},
              // //{.column = 1, .slot = 1, .pattern = new FWF::GrowingStrobePattern(haloMap3d)},
              // //{.column = 1, .slot = 2, .pattern = new Ledster::RadialFadePattern(haloMap3d)},

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
              // //{.column = 5, .slot = 1, .pattern = new Ledster::ChevronsPattern(ledsterMap)},
              // {.column = 5, .slot = 2, .pattern = new FWF3D::ChevronsConePattern(haloMap3d)},
              // //{.column = 5, .slot = 2, .pattern = new Ledster::PixelGlitchPattern()},

              // {.column = 6, .slot = 0, .pattern = new FWF::RibbenFadePattern()},
              // {.column = 6, .slot = 1, .pattern = new FWF::SegmentChasePattern()},
              // //{.column = 6, .slot = 2, .pattern = new FWF::OnBeatColumnFadePattern()},

              // //{.column = 7, .slot = 0, .pattern = new FWF3D::OnBeatColumnChaseUpPattern(ledsterMap3d)},
              // //{.column = 7, .slot = 1, .pattern = new FWF3D::GrowingCirclesPattern(ledsterMap3d)},
              // //{.column = 7, .slot = 2, .pattern = new FWF3D::LineLaunch(ledsterMap3d)},

              {.column = 2, .slot = 0, .pattern = new Mid::Lighthouse(cHaloMap3d)},

              {.column = 4, .slot = 0, .pattern = new Min::RibbenClivePattern<LFOPause<NegativeCosFast>>(10000, 1, 0.15)},
              {.column = 4, .slot = 1, .pattern = new Min::RibbenFlashPattern()},
              {.column = 4, .slot = 2, .pattern = new Min::SegmentChasePattern()},
              {.column = 4, .slot = 3, .pattern = new Min::LineLaunch(haloMap3d)},
              {.column = 4, .slot = 4, .pattern = new Min::GrowingCirclesPattern(haloMap3d)},
              {.column = 4, .slot = 5, .pattern = new Min::GlowPulsePattern()},
              //{.column = 4, .slot = 5, .pattern = new FWF3D::OnBeatColumnChaseUpPattern(haloMap3d)},

              {.column = 5, .slot = 0, .pattern = new Max::RadialGlitterFadePattern(haloMap3d)},
              {.column = 5, .slot = 1, .pattern = new Max::AngularFadePattern(haloMap3d)},
              {.column = 5, .slot = 2, .pattern = new Max::GrowingStrobePattern(cHaloMap3d)},
              {.column = 5, .slot = 3, .pattern = new Max::RadialFadePattern(cHaloMap3d)},
              {.column = 5, .slot = 4, .pattern = new Max::ChevronsPattern(haloMap3d)},
              {.column = 5, .slot = 5, .pattern = new Max::ChevronsConePattern(haloMap3d)},
          }),

      new CloneOutput({new MonitorOutput3dws(haloMap3d, serv),
                       // new MonitorOutput3d(haloMap3d),
                       new UDPOutput("hyperslave4.local", 9611, 60)}));
  hyp->addPipe(haloPipe);
}

void addPaletteColumn(Hyperion *hyp)
{
  auto paletteColumn = new PaletteColumn(
      &hyp->hub,
      0,
      {
          heatmap,
          sunset1,
          sunset3,
          sunset4,
          coralTeal,
          greatBarrierReef,
          campfire,
          tunnel
      });
  hyp->hub.subscribe(paletteColumn);
}