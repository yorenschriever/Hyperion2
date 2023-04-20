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
#include "mapping/columnMap.hpp"
#include "mapping/columnMap3d.hpp"
#include "mapping/haloMap3d.hpp"
#include "mapping/ledsterMap.hpp"
#include "mapping/ledsterMap3d.hpp"
#include "palettes.hpp"
#include "patterns-flash.hpp"
#include "patterns-hi.hpp"
#include "patterns-low.hpp"
#include "patterns-max.hpp"
#include "patterns-mid.hpp"
#include "patterns-min.hpp"
#include "patterns-halo.hpp"
#include "patterns-test.hpp"
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
    /*
    addPaletteColumn(hyp);

    hyp->hub.subscribe(new WebsocketController(&hyp->hub));

    Tempo::AddSource(new ConstantTempo(120));

    // select first palette
    hyp->hub.buttonPressed(0, 0);

    hyp->hub.findColumn(0)->name = "Palette";
    hyp->hub.findColumn(1)->name = "Low";
    hyp->hub.findColumn(2)->name = "Mid";
    hyp->hub.findColumn(3)->name = "Hi";
    hyp->hub.findColumn(4)->name = "Min";
    hyp->hub.findColumn(5)->name = "Max";
    hyp->hub.findColumn(6)->name = "Halo";
    hyp->hub.findColumn(7)->name = "Flash";
    hyp->hub.findColumn(8)->name = "Debug";

    hyp->hub.setFlashColumn(7);
    hyp->hub.setFlashColumn(0, false, true);
*/
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
                // {.column = 2, .slot = 0, .pattern = new Mid::Lighthouse(cLedsterMap3d)},
                // {.column = 2, .slot = 2, .pattern = new Mid::HaloOnBeat(cLedsterMap3d)},
                // {.column = 2, .slot = 3, .pattern = new Mid::SnowflakePatternLedster()},
                // {.column = 2, .slot = 5, .pattern = new Mid::PetalChase(cLedsterMap3d)},

                // {.column = 3, .slot = 0, .pattern = new Hi::SnakePattern()},
                // {.column = 3, .slot = 1, .pattern = new Mid::Halo2(cLedsterMap3d)},
                // {.column = 3, .slot = 2, .pattern = new Hi::XY(ledsterMap3d)},
                // {.column = 3, .slot = 3, .pattern = new Hi::PetalRotatePattern()},
                // {.column = 3, .slot = 4, .pattern = new Hi::HexBeatPattern()},
                // {.column = 3, .slot = 5, .pattern = new Hi::DotBeatPattern(cLedsterMap3d)},

                // {.column = 4, .slot = 0, .pattern = new Min::RibbenClivePattern<LFOPause<NegativeCosFast> >(10000, 1, 0.15)},
                // {.column = 4, .slot = 1, .pattern = new Min::RibbenFlashPattern()},
                // {.column = 4, .slot = 2, .pattern = new Min::SegmentChasePattern()},
                // {.column = 4, .slot = 3, .pattern = new Min::LineLaunch(ledsterMap3d)},
                // {.column = 4, .slot = 4, .pattern = new Min::GrowingCirclesPattern(ledsterMap3d)},
                // {.column = 4, .slot = 5, .pattern = new Min::GlowPulsePattern()},
                // {.column = 4, .slot = 6, .pattern = new Min::SpiralPattern(cLedsterMap3d)},

                // {.column = 5, .slot = 0, .pattern = new Max::RadialGlitterFadePattern(cLedsterMap3d)},
                // {.column = 5, .slot = 1, .pattern = new Max::AngularFadePattern(cLedsterMap3d)},
                // {.column = 5, .slot = 2, .pattern = new Max::GrowingStrobePattern(cLedsterMap3d)},
                // {.column = 5, .slot = 3, .pattern = new Max::RadialFadePattern(cLedsterMap3d)},
                // {.column = 5, .slot = 4, .pattern = new Max::ChevronsPattern(ledsterMap3d)},
                // {.column = 5, .slot = 5, .pattern = new Max::ChevronsConePattern(cLedsterMap3d)},

                // {.column = 7, .slot = 0, .pattern = new Flash::FadingNoisePattern()},
                // {.column = 7, .slot = 1, .pattern = new Flash::SquareGlitchPattern(ledsterMap3d)},
                // {.column = 7, .slot = 2, .pattern = new Flash::FlashesPattern()},
                // {.column = 7, .slot = 3, .pattern = new Flash::StrobePattern()},
                // {.column = 7, .slot = 4, .pattern = new Flash::PixelGlitchPattern()},
                // {.column = 7, .slot = 5, .pattern = new Flash::PetalGlitchPattern()},
                // {.column = 7, .slot = 6, .pattern = new Flash::StrobeHighlightPattern()},
                // {.column = 7, .slot = 7, .pattern = new Max::GrowingStrobePattern(cLedsterMap3d)},

                // {.column = 8, .slot = 0, .pattern = new TestPatterns::ShowStarts(271)},
                // {.column = 8, .slot = 1, .pattern = new TestPatterns::OneColor(RGB(255, 0, 0), "Red")},
                // {.column = 8, .slot = 2, .pattern = new TestPatterns::OneColor(RGB(0, 255, 0), "Green")},
                // {.column = 8, .slot = 3, .pattern = new TestPatterns::OneColor(RGB(0, 0, 255), "Blue")},
                // {.column = 8, .slot = 4, .pattern = new TestPatterns::OneColor(RGB(255, 255, 255), "White")},
                {.column = 8, .slot = 5, .pattern = new TestPatterns::OneColor(RGB(127, 127, 127), "White 50%")},
                
            }),

        new CloneOutput({new MonitorOutput3dws(ledsterMap3d, serv),
                         // new MonitorOutput3d(ledsterMap3d),
                         new UDPOutput("ledsterstandalone.local", 9611, 60)}));
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
            // {.column = 1, .slot = 0, .pattern = new Low::StaticGradientPattern(columnMap3d)},
            // {.column = 1, .slot = 1, .pattern = new Low::OnBeatColumnChaseUpPattern(columnMap3d)},
            // {.column = 1, .slot = 2, .pattern = new Low::HorizontalSin(cColumnMap3d)},
            // {.column = 1, .slot = 3, .pattern = new Low::HorizontalSaw(cColumnMap3d)},
            // {.column = 1, .slot = 4, .pattern = new Low::GrowShrink(cColumnMap3d)},
            // {.column = 1, .slot = 5, .pattern = new Low::GlowPulsePattern(columnMap3d)},
            // {.column = 1, .slot = 6, .pattern = new Low::VerticallyIsolated(cColumnMap3d)},
            // {.column = 1, .slot = 7, .pattern = new Low::RotatingRingsPattern(cColumnMap3d)},

            // {.column = 2, .slot = 0, .pattern = new Mid::Lighthouse(cColumnMap3d)},
            // {.column = 2, .slot = 1, .pattern = new Mid::Halo(cColumnMap3d)},
            // {.column = 2, .slot = 2, .pattern = new Mid::HaloOnBeat(cColumnMap3d)},
            // {.column = 2, .slot = 3, .pattern = new Mid::SnowflakePatternColumn(cColumnMap3d)},
            // {.column = 2, .slot = 4, .pattern = new Mid::TakkenChase(cColumnMap3d)},
            // {.column = 2, .slot = 6, .pattern = new Mid::DoubleFlash(cColumnMap3d)},
            // {.column = 2, .slot = 7, .pattern = new Mid::FireworksPattern(columnMap3d)},

            // {.column = 3, .slot = 2, .pattern = new Hi::XY(columnMap3d)},
            // {.column = 3, .slot = 5, .pattern = new Hi::DotBeatPattern(cColumnMap3d)},

            // {.column = 4, .slot = 0, .pattern = new Min::RibbenClivePattern<LFOPause<NegativeCosFast> >(10000, 1, 0.15)},
            // {.column = 4, .slot = 1, .pattern = new Min::RibbenFlashPattern()},
            // {.column = 4, .slot = 2, .pattern = new Min::SegmentChasePattern()},
            // {.column = 4, .slot = 3, .pattern = new Min::LineLaunch(columnMap3d)},
            // {.column = 4, .slot = 4, .pattern = new Min::GrowingCirclesPattern(columnMap3d)},
            // {.column = 4, .slot = 5, .pattern = new Min::GlowPulsePattern()},
            // {.column = 4, .slot = 6, .pattern = new Min::SpiralPattern(cColumnMap3d)},

            // {.column = 5, .slot = 0, .pattern = new Max::RadialGlitterFadePattern(cColumnMap3d)},
            // {.column = 5, .slot = 1, .pattern = new Max::AngularFadePattern(cColumnMap3d)},
            // {.column = 5, .slot = 2, .pattern = new Max::GrowingStrobePattern(cColumnMap3d)},
            // {.column = 5, .slot = 3, .pattern = new Max::RadialFadePattern(cColumnMap3d)},
            // {.column = 5, .slot = 4, .pattern = new Max::ChevronsPattern(columnMap3d)},
            // {.column = 5, .slot = 5, .pattern = new Max::ChevronsConePattern(cColumnMap3d)},

            // {.column = 7, .slot = 0, .pattern = new Flash::FadingNoisePattern()},
            // {.column = 7, .slot = 1, .pattern = new Flash::SquareGlitchPattern(columnMap3d)},
            // {.column = 7, .slot = 2, .pattern = new Flash::FlashesPattern()},
            // {.column = 7, .slot = 3, .pattern = new Flash::StrobePattern()},
            // {.column = 7, .slot = 4, .pattern = new Flash::PixelGlitchPattern()},
            // {.column = 7, .slot = 6, .pattern = new Flash::StrobeHighlightPattern()},
            // {.column = 7, .slot = 7, .pattern = new Max::GrowingStrobePattern(cColumnMap3d)},

            // {.column = 8, .slot = 0, .pattern = new TestPatterns::ShowStarts(60)},
            // {.column = 8, .slot = 1, .pattern = new TestPatterns::OneColor(RGB(255, 0, 0), "Red")},
            // {.column = 8, .slot = 2, .pattern = new TestPatterns::OneColor(RGB(0, 255, 0), "Green")},
            // {.column = 8, .slot = 3, .pattern = new TestPatterns::OneColor(RGB(0, 0, 255), "Blue")},
            // {.column = 8, .slot = 4, .pattern = new TestPatterns::OneColor(RGB(255, 255, 255), "White")},
            {.column = 8, .slot = 5, .pattern = new TestPatterns::OneColor(RGB(127, 127, 127), "White 50%")},
            
        });

    auto splitInput = new InputSlicer(
        columnsInput,
        {{0 * sizeof(RGBA), 360 * sizeof(RGBA)},
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
        &columnMap3d, {360, 120, 360, 120, 360, 120,
                       360, 120, 360, 120, 360, 120});

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
            haloMap3d.size(),
            &hyp->hub,
            {
                // {.column = 2, .slot = 0, .pattern = new Mid::Lighthouse(cHaloMap3d)},

                // {.column = 4, .slot = 2, .pattern = new Min::SegmentChasePattern()},
                // {.column = 4, .slot = 3, .pattern = new Min::LineLaunch(haloMap3d)},
                // {.column = 4, .slot = 4, .pattern = new Min::GrowingCirclesPattern(haloMap3d)},
                // {.column = 4, .slot = 5, .pattern = new Min::GlowPulsePattern()},
                // {.column = 4, .slot = 6, .pattern = new Min::SpiralPattern(cHaloMap3d)},

                // {.column = 5, .slot = 0, .pattern = new Max::RadialGlitterFadePattern(cHaloMap3d)},
                // {.column = 5, .slot = 1, .pattern = new Max::AngularFadePattern(cHaloMap3d)},
                // {.column = 5, .slot = 2, .pattern = new Max::GrowingStrobePattern(cHaloMap3d)},
                // {.column = 5, .slot = 3, .pattern = new Max::RadialFadePattern(cHaloMap3d)},
                // {.column = 5, .slot = 4, .pattern = new Max::ChevronsPattern(haloMap3d)},
                // {.column = 5, .slot = 5, .pattern = new Max::ChevronsConePattern(cHaloMap3d)},

                // {.column = 6, .slot = 0, .pattern = new Halo::PrimaryColorPattern()},
                // {.column = 6, .slot = 1, .pattern = new Halo::SecondaryColorPattern()},
                // {.column = 6, .slot = 2, .pattern = new Halo::SinChasePattern()},
                // {.column = 6, .slot = 3, .pattern = new Halo::SawChasePattern()},
                // {.column = 6, .slot = 4, .pattern = new Halo::SinAllPattern()},
                // {.column = 6, .slot = 5, .pattern = new Halo::StrobePattern()},


                // {.column = 7, .slot = 0, .pattern = new Flash::FadingNoisePattern()},
                // {.column = 7, .slot = 1, .pattern = new Flash::SquareGlitchPattern(haloMap3d)},
                // {.column = 7, .slot = 2, .pattern = new Flash::FlashesPattern()},
                // {.column = 7, .slot = 3, .pattern = new Flash::StrobePattern()},
                // {.column = 7, .slot = 4, .pattern = new Flash::PixelGlitchPattern()},
                // {.column = 7, .slot = 6, .pattern = new Flash::StrobeHighlightPattern()},
                // {.column = 7, .slot = 7, .pattern = new Max::GrowingStrobePattern(cHaloMap3d)},

                // {.column = 8, .slot = 0, .pattern = new TestPatterns::ShowStarts(100)},
                // {.column = 8, .slot = 1, .pattern = new TestPatterns::OneColor(RGB(255, 0, 0), "Red")},
                // {.column = 8, .slot = 2, .pattern = new TestPatterns::OneColor(RGB(0, 255, 0), "Green")},
                // {.column = 8, .slot = 3, .pattern = new TestPatterns::OneColor(RGB(0, 0, 255), "Blue")},
                // {.column = 8, .slot = 4, .pattern = new TestPatterns::OneColor(RGB(255, 255, 255), "White")},
                {.column = 8, .slot = 5, .pattern = new TestPatterns::OneColor(RGB(127, 127, 127), "White 50%")},

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
            tunnel,

            pinkSunset,
            salmonOnIce,
            blueOrange,
            purpleGreen,
            heatmap2,
            peach,
            deepBlueOcean,
            redSalvation,
            denseWater,
            plumBath,

            retro,
            candy,

            sunset2,
            sunset5,
            sunset6,
            sunset7,
            sunset8,
        });
    hyp->hub.subscribe(paletteColumn);
}