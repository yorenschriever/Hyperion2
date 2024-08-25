#include "core/hyperion.hpp"
#include "mapping/DMXMap3d.hpp"
#include "mapping/singleTriangleMap.hpp"
#include "mapping/triangleMap3d.hpp"
#include "patterns-flash.hpp"
#include "patterns-hi.hpp"
#include "patterns-low.hpp"
#include "patterns-mask.hpp"
#include "patterns-max.hpp"
#include "patterns-mid.hpp"
#include "patterns-min.hpp"
#include "patterns-test.hpp"
#include "setViewParams.hpp"

auto cTriangleMap3d = triangleMap3d.toCylindricalXZ();

LUT *trianglesLut = new ColourCorrectionLUT(1.8, 255, 200, 200, 200);

void addPaletteColumn(Hyperion *hyp);
void addTrianglesPipe(Hyperion *hyp);
void addLedparPipe(Hyperion *hyp);
void addBlinderPipe(Hyperion *hyp);
void addEyesPipe(Hyperion *hyp);

const int LEDPAR_COLUMN = 9;
const int BLINDER_COLUMN = 10;
const int EYES_COLUMN = 11;

int main()
{
    auto hyp = new Hyperion();

    addPaletteColumn(hyp);
    addTrianglesPipe(hyp);
    addLedparPipe(hyp);
    addBlinderPipe(hyp);
    addEyesPipe(hyp);

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
    hyp->hub.setColumnName(LEDPAR_COLUMN, "Led par");
    hyp->hub.setColumnName(BLINDER_COLUMN, "Blinder");
    hyp->hub.setColumnName(EYES_COLUMN, "Eyes");
    hyp->hub.setColumnName(8, "Debug");

    hyp->hub.setFlashColumn(7);
    hyp->hub.setFlashColumn(0, false, true);
    hyp->hub.setForcedSelection(0);

    // hyp->setMidiControllerFactory(new MidiControllerFactoryFvf());

    hyp->start();
    setViewParams(hyp, &viewParamsFront);
    // setViewParams(hyp, viewParamsTop);
    while (1)
        Thread::sleep(1000);
}

void addTrianglesPipe(Hyperion *hyp)
{
    auto trianglesInput = new ControlHubInput<RGBA>(
        triangleMap3d.size(),
        &hyp->hub,
        {
            {.column = 1, .slot = 0, .pattern = new Max::ChevronsPattern(&triangleMap3d)},
            {.column = 1, .slot = 1, .pattern = new Max::ChevronsConePattern(&cTriangleMap3d)},
            {.column = 1, .slot = 2, .pattern = new Max::RadialFadePattern(&cTriangleMap3d)},
            {.column = 1, .slot = 3, .pattern = new Max::RadialGlitterFadePattern(&cTriangleMap3d)},
            {.column = 1, .slot = 4, .pattern = new Max::AngularFadePattern(&cTriangleMap3d)},
            // {.column = 1, .slot = 5, .pattern = new Max::GrowingStrobePattern(&cTriangleMap3d)},

            {.column = 2, .slot = 0, .pattern = new Min::RibbenClivePattern<Glow>(10000, 1, 0.15)},
            {.column = 2, .slot = 1, .pattern = new Min::RibbenFlashPattern()},
            {.column = 2, .slot = 2, .pattern = new Min::GrowingCirclesPattern(&triangleMap3d)},
            {.column = 2, .slot = 3, .pattern = new Min::SpiralPattern(&cTriangleMap3d)},
            {.column = 2, .slot = 4, .pattern = new Min::SegmentChasePattern()},
            {.column = 2, .slot = 5, .pattern = new Min::GlowPulsePattern()},
            {.column = 2, .slot = 6, .pattern = new Min::LineLaunch(&triangleMap3d)},

            {.column = 3, .slot = 0, .pattern = new Low::HorizontalSin(&cTriangleMap3d)},
            {.column = 3, .slot = 1, .pattern = new Low::VerticallyIsolated(&cTriangleMap3d)},
            {.column = 3, .slot = 2, .pattern = new Low::HorizontalSaw(&cTriangleMap3d)},
            {.column = 3, .slot = 3, .pattern = new Low::RadialSaw(&cTriangleMap3d)},
            {.column = 3, .slot = 4, .pattern = new Low::StaticGradientPattern(&triangleMap3d)},
            {.column = 3, .slot = 5, .pattern = new Low::GrowShrink(&cTriangleMap3d)},
            {.column = 3, .slot = 6, .pattern = new Low::RotatingRingsPattern(&cTriangleMap3d)},
            {.column = 3, .slot = 7, .pattern = new Low::GlowPulsePattern(&triangleMap3d)},

            {.column = 4, .slot = 0, .pattern = new Mid::SnowflakePatternColumn(&cTriangleMap3d)},
            {.column = 4, .slot = 1, .pattern = new Mid::TopChase(&cTriangleMap3d)},
            {.column = 4, .slot = 2, .pattern = new Mid::Lighthouse(&cTriangleMap3d)},
            {.column = 4, .slot = 3, .pattern = new Mid::FireworksPattern(&triangleMap3d)},

            {.column = 5, .slot = 0, .pattern = new Hi::DotBeatPattern(&cTriangleMap3d)},
            {.column = 5, .slot = 1, .pattern = new Hi::XY(&triangleMap3d)},
            {.column = 5, .slot = 2, .pattern = new Hi::Z(&triangleMap3d)},

            {.column = 6, .slot = 0, .pattern = new Mask::SinChaseMaskPattern()},
            {.column = 6, .slot = 1, .pattern = new Mask::GlowPulseMaskPattern()},
            {.column = 6, .slot = 2, .pattern = new Mask::XY(&triangleMap3d)},
            {.column = 6, .slot = 3, .pattern = new Mask::HorizontalSaw(&cTriangleMap3d)},
            {.column = 6, .slot = 4, .pattern = new Mask::RadialSaw(&cTriangleMap3d)},
            {.column = 6, .slot = 5, .pattern = new Mask::FireworksPattern(&triangleMap3d)},

            {.column = 7, .slot = 0, .pattern = new Flash::FlashesPattern()},
            {.column = 7, .slot = 1, .pattern = new Flash::SquareGlitchPattern(&triangleMap3d)},
            {.column = 7, .slot = 2, .pattern = new Flash::PixelGlitchPattern()},
            {.column = 7, .slot = 3, .pattern = new Max::GrowingStrobePattern(&cTriangleMap3d)},
            {.column = 7, .slot = 4, .pattern = new Flash::StrobeHighlightPattern()},
            {.column = 7, .slot = 5, .pattern = new Flash::StrobePattern()},
            {.column = 7, .slot = 6, .pattern = new Flash::FadingNoisePattern()},

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

    // // Create 1 inout and split it up in 12,
    // // because UDPOutput is limited by a maximum transfer size of 2*1440 bytes
    // auto splitInput = new InputSlicer(
    //     columnsInput,
    //     {{0 * sizeof(RGBA), 360 * sizeof(RGBA), true},
    //      {360 * sizeof(RGBA), 120 * sizeof(RGBA), true},
    //      {480 * sizeof(RGBA), 360 * sizeof(RGBA), true},
    //      {840 * sizeof(RGBA), 120 * sizeof(RGBA), true},
    //      {960 * sizeof(RGBA), 360 * sizeof(RGBA), true},
    //      {1320 * sizeof(RGBA), 120 * sizeof(RGBA), true},
    //      {1440 * sizeof(RGBA), 360 * sizeof(RGBA), true},
    //      {1800 * sizeof(RGBA), 120 * sizeof(RGBA), true},
    //      {1920 * sizeof(RGBA), 360 * sizeof(RGBA), true},
    //      {2280 * sizeof(RGBA), 120 * sizeof(RGBA), true},
    //      {2400 * sizeof(RGBA), 360 * sizeof(RGBA), true},
    //      {2760 * sizeof(RGBA), 120 * sizeof(RGBA), true},
    //      {0, 6 * 8 * 60 * sizeof(RGBA), false}});

    // typedef struct
    // {
    //     const char *host;
    //     const unsigned short port;
    // } Slave;
    // Slave slaves[] = {
    //     {.host = "hyperslave1.local", .port = 9611}, // rode kolom
    //     {.host = "hyperslave1.local", .port = 9612}, // rode punt
    //     {.host = "hyperslave1.local", .port = 9613}, // groene kolom
    //     {.host = "hyperslave1.local", .port = 9614}, // groene punt
    //     {.host = "hyperslave5.local", .port = 9611}, // blauwe kolom
    //     {.host = "hyperslave5.local", .port = 9612}, // blauwe punt
    //     {.host = "hyperslave5.local", .port = 9613}, // azuur kolom
    //     {.host = "hyperslave5.local", .port = 9614}, // azuur punt
    //     {.host = "hyperslave3.local", .port = 9611}, // paars kolom
    //     {.host = "hyperslave3.local", .port = 9612}, // paars punt
    //     {.host = "hyperslave3.local", .port = 9613}, // geel kolom
    //     {.host = "hyperslave3.local", .port = 9614}, // geel punt
    // };

    // for (int i = 0; i < splitInput->size() - 1; i++)
    // {
    //     auto pipe = new ConvertPipe<RGBA, RGB>(
    //         splitInput->getInput(i),
    //         new UDPOutput(slaves[i].host, slaves[i].port, 60),
    //         trianglesLut);
    //     hyp->addPipe(pipe);
    // }

    hyp->addPipe(
        new ConvertPipe<RGBA, RGB>(
            trianglesInput,
            new MonitorOutput3d(&hyp->webServer, &triangleMap3d)));

    // hyp->addPipe(
    //     new ConvertPipe<RGBA, RGB>(
    //         trianglesInput,
    //         new MonitorOutput(&hyp->webServer,&singleTriangleMap)));
}

void addLedparPipe(Hyperion *hyp)
{
    auto ledparInput = new ControlHubInput<RGBA>(
        ledparMap3d.size(),
        &hyp->hub,
        {
            {.column = LEDPAR_COLUMN, .slot = 0, .pattern = new TestPatterns::ShowStarts(60)},
            {.column = LEDPAR_COLUMN, .slot = 1, .pattern = new TestPatterns::OneColor(RGB(255, 0, 0), "Red")},
            {.column = LEDPAR_COLUMN, .slot = 2, .pattern = new TestPatterns::OneColor(RGB(0, 255, 0), "Green")},
            {.column = LEDPAR_COLUMN, .slot = 3, .pattern = new TestPatterns::OneColor(RGB(0, 0, 255), "Blue")},
            {.column = LEDPAR_COLUMN, .slot = 4, .pattern = new TestPatterns::OneColor(RGB(255, 255, 255), "White")},
            {.column = LEDPAR_COLUMN, .slot = 5, .pattern = new TestPatterns::OneColor(RGB(127, 127, 127), "White 50%")},
            {.column = LEDPAR_COLUMN, .slot = 6, .pattern = new TestPatterns::Palette(120, 20)},
            {.column = LEDPAR_COLUMN, .slot = 7, .pattern = new TestPatterns::Gamma(60)},
            {.column = LEDPAR_COLUMN, .slot = 8, .pattern = new TestPatterns::BrightnessMatch()},
        });

    hyp->addPipe(
        new ConvertPipe<RGBA, RGB>(
            ledparInput,
            new MonitorOutput3d(&hyp->webServer, &ledparMap3d, 60, 0.05)));
}

void addBlinderPipe(Hyperion *hyp)
{
    auto blinderInput = new ControlHubInput<RGBA>(
        blinderMap3d.size(),
        &hyp->hub,
        {
            {.column = BLINDER_COLUMN, .slot = 0, .pattern = new TestPatterns::ShowStarts(60)},
            {.column = BLINDER_COLUMN, .slot = 1, .pattern = new TestPatterns::OneColor(RGB(255, 0, 0), "Red")},
            {.column = BLINDER_COLUMN, .slot = 2, .pattern = new TestPatterns::OneColor(RGB(0, 255, 0), "Green")},
            {.column = BLINDER_COLUMN, .slot = 3, .pattern = new TestPatterns::OneColor(RGB(0, 0, 255), "Blue")},
            {.column = BLINDER_COLUMN, .slot = 4, .pattern = new TestPatterns::OneColor(RGB(255, 255, 255), "White")},
            {.column = BLINDER_COLUMN, .slot = 5, .pattern = new TestPatterns::OneColor(RGB(127, 127, 127), "White 50%")},
            {.column = BLINDER_COLUMN, .slot = 6, .pattern = new TestPatterns::Palette(120, 20)},
            {.column = BLINDER_COLUMN, .slot = 7, .pattern = new TestPatterns::Gamma(60)},
            {.column = BLINDER_COLUMN, .slot = 8, .pattern = new TestPatterns::BrightnessMatch()},
        });

    hyp->addPipe(
        new ConvertPipe<RGBA, RGB>(
            blinderInput,
            new MonitorOutput3d(&hyp->webServer, &blinderMap3d, 60, 0.05)));
}

void addEyesPipe(Hyperion *hyp)
{
    auto eyesInput = new ControlHubInput<RGBA>(
        eyesMap3d.size(),
        &hyp->hub,
        {
            {.column = EYES_COLUMN, .slot = 0, .pattern = new TestPatterns::ShowStarts(60)},
            {.column = EYES_COLUMN, .slot = 1, .pattern = new TestPatterns::OneColor(RGB(255, 0, 0), "Red")},
            {.column = EYES_COLUMN, .slot = 2, .pattern = new TestPatterns::OneColor(RGB(0, 255, 0), "Green")},
            {.column = EYES_COLUMN, .slot = 3, .pattern = new TestPatterns::OneColor(RGB(0, 0, 255), "Blue")},
            {.column = EYES_COLUMN, .slot = 4, .pattern = new TestPatterns::OneColor(RGB(255, 255, 255), "White")},
            {.column = EYES_COLUMN, .slot = 5, .pattern = new TestPatterns::OneColor(RGB(127, 127, 127), "White 50%")},
            {.column = EYES_COLUMN, .slot = 6, .pattern = new TestPatterns::Palette(120, 20)},
            {.column = EYES_COLUMN, .slot = 7, .pattern = new TestPatterns::Gamma(60)},
            {.column = EYES_COLUMN, .slot = 8, .pattern = new TestPatterns::BrightnessMatch()},
        });

    hyp->addPipe(
        new ConvertPipe<RGBA, RGB>(
            eyesInput,
            new MonitorOutput3d(&hyp->webServer, &eyesMap3d, 60, 0.05)));
}

void addPaletteColumn(Hyperion *hyp)
{
    auto paletteColumn = new PaletteColumn(
        &hyp->hub,
        0,
        0,
        {
            &campfire,
            &pinkSunset,
            &sunset8,
            &heatmap,
            &heatmap2,
            &sunset2,
            &retro,
            &tunnel,

            &sunset6,
            &sunset7,
            &sunset1,
            &coralTeal,
            &deepBlueOcean,
            &redSalvation,
            &plumBath,
            &sunset4,
            &candy,
            &sunset3,
            &greatBarrierReef,
            &blueOrange,
            &peach,
            &denseWater,
            &purpleGreen,
            &sunset5,
            &salmonOnIce,
        });
    hyp->hub.subscribe(paletteColumn);
}
