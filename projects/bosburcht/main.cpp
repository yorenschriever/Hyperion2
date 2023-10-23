#include "core/distribution/inputs/inputSlicer.hpp"
#include "core/distribution/luts/colourCorrectionLut.hpp"
#include "core/distribution/outputs/neopixelOutput.hpp"
#include "core/distribution/outputs/spiOutput.hpp"
#include "core/hyperion.hpp"
#include "distribution/outputs/cloneOutput.hpp"
#include "mapping/ceilingMap.hpp"
#include "mapping/ceilingMap3dCombined.hpp"
#include "mapping/chandelierMap3d.hpp"
#include "mapping/chandelierMap3dCombined.hpp"
#include "mapping/windowMap.hpp"
#include "mapping/windowMap3dCombined.hpp"
#include "palettes.hpp"
// #include "patterns.hpp"
#include "core/distribution/luts/colourCorrectionLut.hpp"
#include "core/generation/pixelMapSplitter3d.hpp"
#include "patterns/ceiling.hpp"
#include "patterns/common.hpp"
#include "patterns/window.hpp"
#include "setViewParams.hpp"
#include <vector>

LUT *columnsLut = nullptr; // new ColourCorrectionLUT(1, 255, 200, 200, 200);

auto cwindowMap3dCombined = windowMap3dCombined.toCylindricalXZ(0, chandelierYOffset);
auto cchandelierMap3dCombined = chandelierMap3dCombined.toCylindricalXZ(0, chandelierYOffset);
auto cceilingMap3dCombined = ceilingMap3dCombined.toCylindricalXZ(0, chandelierYOffset);
auto cchandelierMap3d = chandelierMap3d.toCylindricalXZ();

void addPaletteColumn(Hyperion *hyp);

void addWindowPipe(Hyperion *);
void addChandelierPipe(Hyperion *);
void addCeilingPipe(Hyperion *);

#define COL_PALETTE 0
#define COL_WINDOW_BG 1
#define COL_WINDOW_FG 2
#define COL_CHANDELIER_BG 3
#define COL_CHANDELIER_FG 4
#define COL_CEILING_BG 5
#define COL_CEILING_FG 6
#define COL_FLASH 7

typedef struct
{
    const char *host;
    const unsigned short port;
} Slave;

int main()
{
    auto hyp = new Hyperion();

    // hyp->hub.addParams(new Params("second params"));

    addPaletteColumn(hyp);

    addWindowPipe(hyp);
    addChandelierPipe(hyp);
    addCeilingPipe(hyp);

    // addCeilingPipe(hyp);
    Tempo::AddSource(new ConstantTempo(120));

    // select first palette
    hyp->hub.buttonPressed(0, 0);
    hyp->hub.setFlashColumn(0, false, true);
    hyp->hub.setForcedSelection(0);

    hyp->hub.buttonPressed(1, 0);
    // hyp->hub.setFlashColumn(1, false, true);

    hyp->hub.setColumnName(COL_PALETTE, "Palette");
    hyp->hub.setColumnName(COL_WINDOW_BG, "Window bg");
    hyp->hub.setColumnName(COL_WINDOW_FG, "Window fg");
    hyp->hub.setColumnName(COL_CHANDELIER_BG, "Chandelier bg");
    hyp->hub.setColumnName(COL_CHANDELIER_FG, "Chandelier fg");
    hyp->hub.setColumnName(COL_CEILING_BG, "Ceiling bg");
    hyp->hub.setColumnName(COL_CEILING_FG, "Ceiling fg");
    hyp->hub.setColumnName(COL_FLASH, "Flash");

    hyp->start();
    setViewParams(hyp);

    while (1)
        Thread::sleep(60 * 1000);
}

void addWindowPipe(Hyperion *hyp)
{
    int nleds = windowMap3dCombined.size();
    int nbytes = nleds * sizeof(RGBA);
    int segmentSize = nleds / 16;

    auto input = new ControlHubInput<RGBA>(
        windowMap3dCombined.size(),
        &hyp->hub,
        {
            // {.column = 1, .slot = i++, .pattern=new Patterns::Quadrants3d(&windowMap3dCombinedLeft)},

            {.column = COL_WINDOW_BG, .slot = 0, .pattern = new Patterns::RibbenClivePattern<NegativeCosFast>(segmentSize)},
            {.column = COL_WINDOW_BG, .slot = 1, .pattern = new Patterns::RibbenFlashPattern(segmentSize)},
            {.column = COL_WINDOW_BG, .slot = 2, .pattern = new Patterns::GrowShrink(&windowMap3dCombined)},
            {.column = COL_WINDOW_BG, .slot = 3, .pattern = new Patterns::VerticallyIsolated(&windowMap3dCombined)},
            {.column = COL_WINDOW_BG, .slot = 4, .pattern = new Patterns::RotatingRingsPattern(&windowMap3dCombined)},
            {.column = COL_WINDOW_BG, .slot = 5, .pattern = new Patterns::OnBeatWindowChaseUpPattern(&windowMap3dCombined)},

            {.column = COL_WINDOW_FG, .slot = 0, .pattern = new Patterns::GlowPulsePattern()},

            {.column = COL_FLASH, .slot = 0, .pattern = new Patterns::GlitchPattern(segmentSize)},
            {.column = COL_FLASH, .slot = 1, .pattern = new Patterns::PixelGlitchPattern()},
            {.column = COL_FLASH, .slot = 2, .pattern = new Patterns::Lighthouse(&cwindowMap3dCombined)},
            {.column = COL_FLASH, .slot = 3, .pattern = new Patterns::RadialGlitterFadePattern(&cwindowMap3dCombined)},
            {.column = COL_FLASH, .slot = 4, .pattern = new Patterns::RadialFadePattern(&cwindowMap3dCombined)},
        });

    auto splitInput = new InputSlicer(
        input,
        {
            {0 * nbytes / 16, nbytes / 16, true},
            {1 * nbytes / 16, nbytes / 16, true},
            {2 * nbytes / 16, nbytes / 16, true},
            {3 * nbytes / 16, nbytes / 16, true},
            {4 * nbytes / 16, nbytes / 16, true},
            {5 * nbytes / 16, nbytes / 16, true},
            {6 * nbytes / 16, nbytes / 16, true},
            {7 * nbytes / 16, nbytes / 16, true},
            {8 * nbytes / 16, nbytes / 16, true},
            {9 * nbytes / 16, nbytes / 16, true},
            {10 * nbytes / 16, nbytes / 16, true},
            {11 * nbytes / 16, nbytes / 16, true},
            {12 * nbytes / 16, nbytes / 16, true},
            {13 * nbytes / 16, nbytes / 16, true},
            {14 * nbytes / 16, nbytes / 16, true},
            {15 * nbytes / 16, nbytes / 16, true},

            {0, nbytes / 2, false},
            {nbytes / 2, nbytes / 2, false},
        });

    auto splitMap = new PixelMapSplitter3d(&windowMap3dCombined, {nleds / 2, nleds / 2});

    Slave slaves[] = {
        {.host = "hyperslave1.local", .port = 9611},
        {.host = "hyperslave1.local", .port = 9612},
        {.host = "hyperslave1.local", .port = 9613},
        {.host = "hyperslave1.local", .port = 9614},

        {.host = "hyperslave2.local", .port = 9611},
        {.host = "hyperslave2.local", .port = 9612},
        {.host = "hyperslave2.local", .port = 9613},
        {.host = "hyperslave2.local", .port = 9614},

        {.host = "hyperslave3.local", .port = 9611},
        {.host = "hyperslave3.local", .port = 9612},
        {.host = "hyperslave3.local", .port = 9613},
        {.host = "hyperslave3.local", .port = 9614},

        {.host = "hyperslave4.local", .port = 9611},
        {.host = "hyperslave4.local", .port = 9612},
        {.host = "hyperslave4.local", .port = 9613},
        {.host = "hyperslave4.local", .port = 9614},
    };

    for (int i = 0; i < splitInput->size() - 2; i++)
    {
        auto pipe = new ConvertPipe<RGBA, BGR>(
            splitInput->getInput(i),
            new UDPOutput(slaves[i].host, slaves[i].port, 60),
            columnsLut);
        hyp->addPipe(pipe);
    }

    hyp->addPipe(
        new ConvertPipe<RGBA, RGB>(
            splitInput->getInput(splitInput->size() - 2),
            new MonitorOutput3d(&hyp->webServer, splitMap->getMap(0))));
    hyp->addPipe(
        new ConvertPipe<RGBA, RGB>(
            splitInput->getInput(splitInput->size() - 1),
            new MonitorOutput3d(&hyp->webServer, splitMap->getMap(1))));
}

void addChandelierPipe(Hyperion *hyp)
{
    int nleds = chandelierMap3dCombined.size();
    int nbytes = nleds * sizeof(RGBA);

    auto input = new ControlHubInput<RGBA>(
        nleds,
        &hyp->hub,
        {
            // {.column = 1, .slot = i++, .pattern=new Patterns::Quadrants3d(&chandelierMap3dCombined)},

            {.column = COL_CHANDELIER_BG, .slot = 0, .pattern = new Patterns::RibbenClivePattern<NegativeCosFast>()},
            {.column = COL_CHANDELIER_BG, .slot = 1, .pattern = new Patterns::RibbenFlashPattern()},

            {.column = COL_CHANDELIER_FG, .slot = 0, .pattern = new Patterns::GlowPulsePattern()},
            {.column = COL_CHANDELIER_FG, .slot = 1, .pattern = new Patterns::SegmentChasePattern()},
            {.column = COL_CHANDELIER_FG, .slot = 2, .pattern = new Patterns::Lighthouse(&cchandelierMap3d)},

            {.column = COL_FLASH, .slot = 0, .pattern = new Patterns::GlitchPattern()},
            {.column = COL_FLASH, .slot = 1, .pattern = new Patterns::PixelGlitchPattern()},
            {.column = COL_FLASH, .slot = 2, .pattern = new Patterns::Lighthouse(&cchandelierMap3dCombined)},
            {.column = COL_FLASH, .slot = 3, .pattern = new Patterns::RadialGlitterFadePattern(&cchandelierMap3dCombined)},
            {.column = COL_FLASH, .slot = 4, .pattern = new Patterns::RadialFadePattern(&cchandelierMap3dCombined)},
        });

    auto splitInput = new InputSlicer(input,
                                      {
                                          {0 * nbytes / 8, nbytes / 8, true},
                                          {1 * nbytes / 8, nbytes / 8, true},
                                          {2 * nbytes / 8, nbytes / 8, true},
                                          {3 * nbytes / 8, nbytes / 8, true},
                                          {4 * nbytes / 8, nbytes / 8, true},
                                          {5 * nbytes / 8, nbytes / 8, true},
                                          {6 * nbytes / 8, nbytes / 8, true},
                                          {7 * nbytes / 8, nbytes / 8, true},
                                          {0, nbytes, false},
                                      });

    for (int i = 0; i < splitInput->size() - 1; i++)
    {
        auto pipe = new ConvertPipe<RGBA, BGR>(
            splitInput->getInput(i),
            new UDPOutput("hyperslave5.local", 9611 + i, 60),
            columnsLut);
        hyp->addPipe(pipe);
    }

    hyp->addPipe(
        new ConvertPipe<RGBA, RGB>(
            splitInput->getInput(splitInput->size() - 1),
            new MonitorOutput3d(&hyp->webServer, &chandelierMap3dCombined)));
}

void addCeilingPipe(Hyperion *hyp)
{
    auto input = new ControlHubInput<RGBA>(
        ceilingMap3dCombined.size(),
        &hyp->hub,
        {
            // {.column = 1, .slot = i++, .pattern=new Patterns::Quadrants3d(&ceilingMap3dCombined)},

            // {.column = 1, .slot = i++, .pattern=new Patterns::Lighthouse(&cceilingMap3dCombined)},
            // {.column = 1, .slot = 4, .pattern=new Patterns::IndexMapTest()},
            // {.column = 1, .slot = 5, .pattern=new Patterns::CeilingChase()},

            {.column = COL_CEILING_BG, .slot = 0, .pattern = new Patterns::RibbenClivePattern<NegativeCosFast>()},
            {.column = COL_CEILING_BG, .slot = 1, .pattern = new Patterns::RibbenFlashPattern()},
            {.column = COL_CEILING_BG, .slot = 2, .pattern = new Patterns::CeilingChase()},

            {.column = COL_CEILING_FG, .slot = 0, .pattern = new Patterns::GlowPulsePattern()},
            {.column = COL_CEILING_FG, .slot = 1, .pattern = new Patterns::SegmentChasePattern()},

            {.column = COL_FLASH, .slot = 0, .pattern = new Patterns::GlitchPattern()},
            {.column = COL_FLASH, .slot = 1, .pattern = new Patterns::PixelGlitchPattern()},
            {.column = COL_FLASH, .slot = 2, .pattern = new Patterns::Lighthouse(&cceilingMap3dCombined)},
            {.column = COL_FLASH, .slot = 3, .pattern = new Patterns::RadialGlitterFadePattern(&cceilingMap3dCombined)},
            {.column = COL_FLASH, .slot = 4, .pattern = new Patterns::RadialFadePattern(&cceilingMap3dCombined)},

        });

    hyp->addPipe(new ConvertPipe<RGBA, RGB>(
        input,
        new CloneOutput({
            new MonitorOutput3d(&hyp->webServer, &ceilingMap3dCombined),
            // new MonitorOutput3d(&hyp->webServer, &sunMap3d)
        })));
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
