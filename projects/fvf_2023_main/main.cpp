#include "colours.h"
#include "core/distribution/inputs/inputSlicer.hpp"
#include "core/distribution/luts/colourCorrectionLut.hpp"
#include "core/distribution/outputs/monitorOutput3d.hpp"
#include "core/distribution/outputs/udpOutput.hpp"
#include "core/distribution/pipes/convertPipe.hpp"
#include "core/generation/patterns/helpers/tempo/constantTempo.h"
#include "core/generation/pixelMap.hpp"
#include "core/hyperion.hpp"
#include "distribution/inputs/controlHubInput.hpp"
#include "generation/controlHub/paletteColumn.hpp"
#include "mapping/columnMap3d.hpp"
#include "mapping/haloMap3d.hpp"
#include "mapping/ledsterMap3d.hpp"
#include "palettes.hpp"
#include "patterns-flash.hpp"
#include "patterns-halo.hpp"
#include "patterns-hi.hpp"
#include "patterns-low.hpp"
#include "patterns-max.hpp"
#include "patterns-mid.hpp"
#include "patterns-min.hpp"
#include "patterns-test.hpp"
#include "thread.hpp"
#include "webServer.hpp"
#include "webServerResponseBuilder.hpp"
#include "websocketServer.hpp"
#include <algorithm>
#include <iostream>
#include <iterator>
#include "palette-midi.hpp"

// op het oog is een gamma van 1.8 het mooist, maar vanwege de kleur resolutie toch 1.4 gekozen
LUT *ledsterLut = new ColourCorrectionLUT(1.5, 255, 255, 255, 240);
LUT *columnsLut = new ColourCorrectionLUT(1, 255, 200, 200, 200);
LUT *haloLut = nullptr; // new ColourCorrectionLUT(1, 255, 255, 255, 255);

auto cColumnMap3d = columnMap3d.toCylindricalXZ();
auto cLedsterMap3d = ledsterMap3d.toCylindricalXZ();
auto cHaloMap3d = haloMap3d.toCylindricalXZ();

void addLedsterPipe(Hyperion *hyp);
void addColumnPipes(Hyperion *hyp);
void addHaloPipe(Hyperion *hyp);
void addPaletteColumn(Hyperion *hyp);

int main()
{
    auto hyp = new Hyperion();

    addColumnPipes(hyp);
    addLedsterPipe(hyp);
    addHaloPipe(hyp);
    addPaletteColumn(hyp);

    Tempo::AddSource(new ConstantTempo(120));

    // select first palette
    hyp->hub.buttonPressed(0, 0);

    hyp->hub.setColumnName(0, "Palette");
    hyp->hub.setColumnName(1, "Max");
    hyp->hub.setColumnName(2, "Min");
    hyp->hub.setColumnName(3, "Low");
    hyp->hub.setColumnName(4, "Mid");
    hyp->hub.setColumnName(5, "Hi");
    hyp->hub.setColumnName(6, "Halo");
    hyp->hub.setColumnName(7, "Flash");
    hyp->hub.setColumnName(8, "Debug");

    hyp->hub.setFlashColumn(7);
    hyp->hub.setFlashColumn(0, false, true);
    hyp->hub.setForcedSelection(0);

    hyp->midiControllerFactory = new MidiControllerFactoryFvf();

    hyp->start();
    while (1)
        Thread::sleep(1000);
}

void addLedsterPipe(Hyperion *hyp)
{
    auto input = new ControlHubInput<RGBA>(
        ledsterMap3d.size(),
        &hyp->hub,
        {
            {.column = 1, .slot = 0, .pattern = new Max::ChevronsPattern(&ledsterMap3d)},
            {.column = 1, .slot = 1, .pattern = new Max::ChevronsConePattern(&cLedsterMap3d)},
            {.column = 1, .slot = 2, .pattern = new Max::RadialFadePattern(&cLedsterMap3d)},
            {.column = 1, .slot = 3, .pattern = new Max::RadialGlitterFadePattern(&cLedsterMap3d)},
            {.column = 1, .slot = 4, .pattern = new Max::AngularFadePattern(&cLedsterMap3d)},
            {.column = 1, .slot = 5, .pattern = new Max::GrowingStrobePattern(&cLedsterMap3d)},

            {.column = 2, .slot = 0, .pattern = new Min::RibbenClivePattern<Glow>(10000, 1, 0.15)},
            {.column = 2, .slot = 1, .pattern = new Min::RibbenFlashPattern()},
            {.column = 2, .slot = 2, .pattern = new Min::GrowingCirclesPattern(&ledsterMap3d)},
            {.column = 2, .slot = 3, .pattern = new Min::SpiralPattern(&cLedsterMap3d)},
            {.column = 2, .slot = 4, .pattern = new Min::SegmentChasePattern()},
            {.column = 2, .slot = 5, .pattern = new Min::GlowPulsePattern()},
            {.column = 2, .slot = 6, .pattern = new Min::LineLaunch(&ledsterMap3d)},

            {.column = 4, .slot = 0, .pattern = new Mid::HaloOnBeat(&cLedsterMap3d)},
            {.column = 4, .slot = 2, .pattern = new Mid::SnowflakePatternLedster()},
            {.column = 4, .slot = 4, .pattern = new Mid::PetalChase(&cLedsterMap3d)},
            {.column = 4, .slot = 5, .pattern = new Mid::Lighthouse(&cLedsterMap3d)},

            {.column = 5, .slot = 0, .pattern = new Hi::DotBeatPattern(&cLedsterMap3d)},
            {.column = 5, .slot = 1, .pattern = new Mid::Halo2(&cLedsterMap3d)},
            {.column = 5, .slot = 2, .pattern = new Hi::PetalRotatePattern()},
            {.column = 5, .slot = 3, .pattern = new Hi::SnakePattern()},
            {.column = 5, .slot = 4, .pattern = new Hi::HexBeatPattern()},
            {.column = 5, .slot = 5, .pattern = new Hi::XY(&ledsterMap3d)},

            {.column = 7, .slot = 0, .pattern = new Flash::FlashesPattern()},
            {.column = 7, .slot = 1, .pattern = new Flash::SquareGlitchPattern(&ledsterMap3d)},
            {.column = 7, .slot = 2, .pattern = new Flash::PetalGlitchPattern()},
            {.column = 7, .slot = 3, .pattern = new Flash::PixelGlitchPattern()},
            {.column = 7, .slot = 4, .pattern = new Max::GrowingStrobePattern(&cLedsterMap3d)},
            {.column = 7, .slot = 5, .pattern = new Flash::StrobeHighlightPattern()},
            {.column = 7, .slot = 6, .pattern = new Flash::StrobePattern()},
            {.column = 7, .slot = 7, .pattern = new Flash::FadingNoisePattern()},

            {.column = 8, .slot = 0, .pattern = new TestPatterns::ShowStarts(271)},
            {.column = 8, .slot = 1, .pattern = new TestPatterns::OneColor(RGB(255, 0, 0), "Red")},
            {.column = 8, .slot = 2, .pattern = new TestPatterns::OneColor(RGB(0, 255, 0), "Green")},
            {.column = 8, .slot = 3, .pattern = new TestPatterns::OneColor(RGB(0, 0, 255), "Blue")},
            {.column = 8, .slot = 4, .pattern = new TestPatterns::OneColor(RGB(255, 255, 255), "White")},
            {.column = 8, .slot = 5, .pattern = new TestPatterns::OneColor(RGB(127, 127, 127), "White 50%")},
            {.column = 8, .slot = 6, .pattern = new TestPatterns::Palette(10, 1)},
            {.column = 8, .slot = 7, .pattern = new TestPatterns::Gamma(10)},
            {.column = 8, .slot = 8, .pattern = new TestPatterns::BrightnessMatch()},
        });

    auto splitInput = new InputSlicer(input, {
        {0, int(ledsterMap3d.size() * sizeof(RGBA)), true},
        {0, int(ledsterMap3d.size() * sizeof(RGBA)), false}
    });

    // hyp->addPipe(new ConvertPipe<RGBA, RGB>(
    //     splitInput->getInput(0),
    //     new UDPOutput("ledsterstandalone.local", 9601, 60),
    //     ledsterLut));

    hyp->addPipe(new ConvertPipe<RGBA, RGB>(
        splitInput->getInput(0),
        new UDPOutput("hyperslave2.local", 9611, 60),
        ledsterLut));

    hyp->addPipe(new ConvertPipe<RGBA, RGB>(
        splitInput->getInput(1),
        new MonitorOutput3d(&hyp->webServer,&ledsterMap3d)));
}

void addColumnPipes(Hyperion *hyp)
{
    auto columnsInput = new ControlHubInput<RGBA>(
        columnMap3d.size(),
        &hyp->hub,
        {
            {.column = 1, .slot = 0, .pattern = new Max::ChevronsPattern(&columnMap3d)},
            {.column = 1, .slot = 1, .pattern = new Max::ChevronsConePattern(&cColumnMap3d)},
            {.column = 1, .slot = 2, .pattern = new Max::RadialFadePattern(&cColumnMap3d)},
            {.column = 1, .slot = 3, .pattern = new Max::RadialGlitterFadePattern(&cColumnMap3d)},
            {.column = 1, .slot = 4, .pattern = new Max::AngularFadePattern(&cColumnMap3d)},
            {.column = 1, .slot = 5, .pattern = new Max::GrowingStrobePattern(&cColumnMap3d)},

            {.column = 2, .slot = 0, .pattern = new Min::RibbenClivePattern<Glow>(10000, 1, 0.15)},
            {.column = 2, .slot = 1, .pattern = new Min::RibbenFlashPattern()},
            {.column = 2, .slot = 2, .pattern = new Min::GrowingCirclesPattern(&columnMap3d)},
            {.column = 2, .slot = 3, .pattern = new Min::SpiralPattern(&cColumnMap3d)},
            {.column = 2, .slot = 4, .pattern = new Min::SegmentChasePattern()},
            {.column = 2, .slot = 5, .pattern = new Min::GlowPulsePattern()},
            {.column = 2, .slot = 6, .pattern = new Min::LineLaunch(&columnMap3d)},

            {.column = 3, .slot = 0, .pattern = new Low::HorizontalSin(&cColumnMap3d)},
            {.column = 3, .slot = 1, .pattern = new Low::VerticallyIsolated(&cColumnMap3d)},
            {.column = 3, .slot = 2, .pattern = new Low::HorizontalSaw(&cColumnMap3d)},
            {.column = 3, .slot = 3, .pattern = new Low::StaticGradientPattern(&columnMap3d)},
            {.column = 3, .slot = 4, .pattern = new Low::OnBeatColumnChaseUpPattern(&columnMap3d)},
            {.column = 3, .slot = 5, .pattern = new Low::GrowShrink(&cColumnMap3d)},
            {.column = 3, .slot = 6, .pattern = new Low::RotatingRingsPattern(&cColumnMap3d)},
            {.column = 3, .slot = 7, .pattern = new Low::GlowPulsePattern(&columnMap3d)},

            {.column = 4, .slot = 0, .pattern = new Mid::HaloOnBeat(&cColumnMap3d)},
            {.column = 4, .slot = 1, .pattern = new Mid::Halo(&cColumnMap3d)},
            {.column = 4, .slot = 2, .pattern = new Mid::SnowflakePatternColumn(&cColumnMap3d)},
            {.column = 4, .slot = 3, .pattern = new Mid::TakkenChase(&cColumnMap3d)},
            {.column = 4, .slot = 5, .pattern = new Mid::Lighthouse(&cColumnMap3d)},
            {.column = 4, .slot = 6, .pattern = new Mid::FireworksPattern(&columnMap3d)},
            {.column = 4, .slot = 7, .pattern = new Mid::DoubleFlash(&cColumnMap3d)},

            {.column = 5, .slot = 0, .pattern = new Hi::DotBeatPattern(&cColumnMap3d)},
            {.column = 5, .slot = 5, .pattern = new Hi::XY(&columnMap3d)},

            {.column = 7, .slot = 0, .pattern = new Flash::FlashesPattern()},
            {.column = 7, .slot = 1, .pattern = new Flash::SquareGlitchPattern(&columnMap3d)},
            {.column = 7, .slot = 3, .pattern = new Flash::PixelGlitchPattern()},
            {.column = 7, .slot = 4, .pattern = new Max::GrowingStrobePattern(&cColumnMap3d)},
            {.column = 7, .slot = 5, .pattern = new Flash::StrobeHighlightPattern()},
            {.column = 7, .slot = 6, .pattern = new Flash::StrobePattern()},
            {.column = 7, .slot = 7, .pattern = new Flash::FadingNoisePattern()},

            {.column = 8, .slot = 0, .pattern = new TestPatterns::ShowStarts(60)},
            {.column = 8, .slot = 1, .pattern = new TestPatterns::OneColor(RGB(255, 0, 0), "Red")},
            {.column = 8, .slot = 2, .pattern = new TestPatterns::OneColor(RGB(0, 255, 0), "Green")},
            {.column = 8, .slot = 3, .pattern = new TestPatterns::OneColor(RGB(0, 0, 255), "Blue")},
            {.column = 8, .slot = 4, .pattern = new TestPatterns::OneColor(RGB(255, 255, 255), "White")},
            {.column = 8, .slot = 5, .pattern = new TestPatterns::OneColor(RGB(127, 127, 127), "White 50%")},
            {.column = 8, .slot = 6, .pattern = new TestPatterns::Palette(120, 20)},
            {.column = 8, .slot = 7, .pattern = new TestPatterns::Gamma(60)},
            {.column = 8, .slot = 8, .pattern = new TestPatterns::BrightnessMatch()},
        });

    // Create 1 inout and split it up in 12,
    // because UDPOutput is limited by a maximum transfer size of 2*1440 bytes
    auto splitInput = new InputSlicer(
        columnsInput,
        {{0 * sizeof(RGBA), 360 * sizeof(RGBA), true},
         {360 * sizeof(RGBA), 120 * sizeof(RGBA), true},
         {480 * sizeof(RGBA), 360 * sizeof(RGBA), true},
         {840 * sizeof(RGBA), 120 * sizeof(RGBA), true},
         {960 * sizeof(RGBA), 360 * sizeof(RGBA), true},
         {1320 * sizeof(RGBA), 120 * sizeof(RGBA), true},
         {1440 * sizeof(RGBA), 360 * sizeof(RGBA), true},
         {1800 * sizeof(RGBA), 120 * sizeof(RGBA), true},
         {1920 * sizeof(RGBA), 360 * sizeof(RGBA), true},
         {2280 * sizeof(RGBA), 120 * sizeof(RGBA), true},
         {2400 * sizeof(RGBA), 360 * sizeof(RGBA), true},
         {2760 * sizeof(RGBA), 120 * sizeof(RGBA), true},
         {0, 6 * 8 * 60 * sizeof(RGBA), false}});

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
        {.host = "hyperslave5.local", .port = 9611}, // blauwe kolom
        {.host = "hyperslave5.local", .port = 9612}, // blauwe punt
        {.host = "hyperslave5.local", .port = 9613}, // azuur kolom
        {.host = "hyperslave5.local", .port = 9614}, // azuur punt
        {.host = "hyperslave3.local", .port = 9611}, // paars kolom
        {.host = "hyperslave3.local", .port = 9612}, // paars punt
        {.host = "hyperslave3.local", .port = 9613}, // geel kolom
        {.host = "hyperslave3.local", .port = 9614}, // geel punt
    };

    for (int i = 0; i < splitInput->size() - 1; i++)
    {
        auto pipe = new ConvertPipe<RGBA, RGB>(
            splitInput->getInput(i),
            new UDPOutput(slaves[i].host, slaves[i].port, 60),
            columnsLut);
        hyp->addPipe(pipe);
    }

    hyp->addPipe(
        new ConvertPipe<RGBA, RGB>(
            splitInput->getInput(12),
            new MonitorOutput3d(&hyp->webServer,&columnMap3d)));
}

void addHaloPipe(Hyperion *hyp)
{
    //auto haloPipe = new ConvertPipe<RGBA, RGB>(
    auto input = new ControlHubInput<RGBA>(
            haloMap3d.size(),
            &hyp->hub,
            {
                {.column = 1, .slot = 0, .pattern = new Max::ChevronsPattern(&haloMap3d)},
                {.column = 1, .slot = 1, .pattern = new Max::ChevronsConePattern(&cHaloMap3d)},
                {.column = 1, .slot = 2, .pattern = new Max::RadialFadePattern(&cHaloMap3d)},
                {.column = 1, .slot = 3, .pattern = new Max::RadialGlitterFadePattern(&cHaloMap3d)},
                {.column = 1, .slot = 4, .pattern = new Max::AngularFadePattern(&cHaloMap3d)},
                {.column = 1, .slot = 5, .pattern = new Max::GrowingStrobePattern(&cHaloMap3d)},

                {.column = 2, .slot = 2, .pattern = new Min::GrowingCirclesPattern(&haloMap3d)},
                {.column = 2, .slot = 3, .pattern = new Min::SpiralPattern(&cHaloMap3d)},
                {.column = 2, .slot = 4, .pattern = new Min::SegmentChasePattern()},
                {.column = 2, .slot = 5, .pattern = new Min::GlowPulsePattern()},
                {.column = 2, .slot = 6, .pattern = new Min::LineLaunch(&haloMap3d)},

                {.column = 4, .slot = 5, .pattern = new Mid::Lighthouse(&cHaloMap3d)},

                {.column = 6, .slot = 0, .pattern = new Halo::PrimaryColorPattern()},
                {.column = 6, .slot = 1, .pattern = new Halo::SecondaryColorPattern()},
                {.column = 6, .slot = 2, .pattern = new Halo::SinAllPattern()},
                {.column = 6, .slot = 3, .pattern = new Halo::SinChasePattern()},
                {.column = 6, .slot = 4, .pattern = new Halo::SawChasePattern()},
                {.column = 6, .slot = 5, .pattern = new Halo::StrobePattern()},

                {.column = 7, .slot = 0, .pattern = new Flash::FlashesPattern()},
                {.column = 7, .slot = 1, .pattern = new Flash::SquareGlitchPattern(&haloMap3d)},
                {.column = 7, .slot = 3, .pattern = new Flash::PixelGlitchPattern()},
                {.column = 7, .slot = 4, .pattern = new Max::GrowingStrobePattern(&cHaloMap3d)},
                {.column = 7, .slot = 5, .pattern = new Flash::StrobeHighlightPattern()},
                {.column = 7, .slot = 6, .pattern = new Flash::StrobePattern()},
                {.column = 7, .slot = 7, .pattern = new Flash::FadingNoisePattern()},

                {.column = 8, .slot = 0, .pattern = new TestPatterns::ShowStarts(haloMap3d.size())},
                {.column = 8, .slot = 1, .pattern = new TestPatterns::OneColor(RGB(255, 0, 0), "Red")},
                {.column = 8, .slot = 2, .pattern = new TestPatterns::OneColor(RGB(0, 255, 0), "Green")},
                {.column = 8, .slot = 3, .pattern = new TestPatterns::OneColor(RGB(0, 0, 255), "Blue")},
                {.column = 8, .slot = 4, .pattern = new TestPatterns::OneColor(RGB(255, 255, 255), "White")},
                {.column = 8, .slot = 5, .pattern = new TestPatterns::OneColor(RGB(127, 127, 127), "White 50%")},
                {.column = 8, .slot = 6, .pattern = new TestPatterns::Palette(81, 5)},
                {.column = 8, .slot = 7, .pattern = new TestPatterns::Gamma(haloMap3d.size())},
                {.column = 8, .slot = 8, .pattern = new TestPatterns::BrightnessMatch()},
            });

    auto splitInput = new InputSlicer(input, {
        {0, int(haloMap3d.size() * sizeof(RGBA)), true}, 
        {0, int(haloMap3d.size() * sizeof(RGBA)), false}
        });

    hyp->addPipe(new ConvertPipe<RGBA, RBG>(
        splitInput->getInput(0),
        new UDPOutput("haloslave.local", 9611, 60),
        haloLut));

    hyp->addPipe(new ConvertPipe<RGBA, RGB>(
        splitInput->getInput(1),
        new MonitorOutput3d(&hyp->webServer,&haloMap3d)));
}

void addPaletteColumn(Hyperion *hyp)
{
    auto paletteColumn = new PaletteColumn(
        &hyp->hub,
        0,
        0,
        {
            campfire,
            pinkSunset,
            sunset8,
            heatmap,
            heatmap2,
            sunset2,
            retro,
            tunnel,

            sunset6,
            sunset7,
            sunset1,
            coralTeal,
            deepBlueOcean,
            redSalvation,
            plumBath,
            sunset4,
            candy,
            sunset3,
            greatBarrierReef,
            blueOrange,
            peach,
            denseWater,
            purpleGreen,
            sunset5,
            salmonOnIce,
        });
    hyp->hub.subscribe(paletteColumn);
}

