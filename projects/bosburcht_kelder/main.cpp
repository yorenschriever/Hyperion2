#include "core/distribution/inputs/inputSlicer.hpp"
#include "core/distribution/luts/colourCorrectionLut.hpp"
#include "core/distribution/outputs/neopixelOutput.hpp"
#include "core/distribution/outputs/spiOutput.hpp"
#include "core/hyperion.hpp"
#include "distribution/outputs/cloneOutput.hpp"
#include "mapping/ceilingMap.hpp"
#include "mapping/ceilingMap3dCombined.hpp"
#include "mapping/colanderMap.hpp"
#include "mapping/colanderMap3dCombined.hpp"
#include "palettes.hpp"
#include "core/distribution/luts/colourCorrectionLut.hpp"
#include "core/generation/pixelMapSplitter3d.hpp"
#include "patterns/ceiling.hpp"
#include "patterns/common.hpp"
#include "patterns/window.hpp"
#include "patterns/chandelier.hpp"
#include "setViewParams.hpp"
#include <vector>

LUT *columnsLut = nullptr; // new ColourCorrectionLUT(1, 255, 200, 200, 200);

void addPaletteColumn(Hyperion *hyp);

void addColanderPipe(Hyperion *);
void addCeilingPipe(Hyperion *);

#define COL_PALETTE 0
#define COL_WINDOW 1
#define COL_COLANDER 2
#define COL_CEILING 3
#define COL_ALL 4
#define COL_MASK 5
// #define COL_UNUSED 6
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

    addColanderPipe(hyp);
    addCeilingPipe(hyp);

    Tempo::AddSource(new ConstantTempo(120));

    // select first palette
    hyp->hub.buttonPressed(0, 0);
    hyp->hub.setFlashColumn(0, false, true);
    hyp->hub.setForcedSelection(0);

    //hyp->hub.buttonPressed(1, 0);
    hyp->hub.setFlashColumn(COL_FLASH, true, false);

    hyp->hub.setColumnName(COL_PALETTE, "Palette");
    hyp->hub.setColumnName(COL_WINDOW, "Window bg");
    hyp->hub.setColumnName(COL_COLANDER, "Colander");
    hyp->hub.setColumnName(COL_CEILING, "Ceiling");
    hyp->hub.setColumnName(COL_ALL, "All");
    hyp->hub.setColumnName(COL_MASK, "Mask");
    hyp->hub.setColumnName(COL_FLASH, "Flash");

    hyp->start();
    setViewParams(hyp);

    while (1)
        Thread::sleep(60 * 1000);
}

void addCeilingPipe(Hyperion *hyp)
{
    int nleds = ceilingMap3dCombined.size();
    int nbytes = nleds * sizeof(RGBA);

    auto input = new ControlHubInput<RGBA>(
        nleds,
        &hyp->hub,
        {
            // {.column = 1, .slot = i++, .pattern=new Patterns::Quadrants3d(&ceilingMap3dCombined)},

            // {.column = 1, .slot = i++, .pattern=new Patterns::Lighthouse(&cceilingMap3dCombined)},
            // {.column = 1, .slot = 4, .pattern=new Patterns::IndexMapTest()},
            // {.column = 1, .slot = 5, .pattern=new Patterns::CeilingChase()},

            {.column = COL_CEILING, .slot = 0, .pattern = new Patterns::RibbenClivePattern<NegativeCosFast>()},
            {.column = COL_CEILING, .slot = 1, .pattern = new Patterns::RibbenFlashPattern()},
            // {.column = COL_CEILING, .slot = 2, .pattern = new Patterns::CeilingChase()},
            // {.column = COL_CEILING, .slot = 3, .pattern = new Patterns::SinChasePattern()},
            // {.column = COL_CEILING, .slot = 4, .pattern = new Patterns::SawChasePattern()},
            {.column = COL_CEILING, .slot = 5, .pattern = new Patterns::FadeFromCenter()},
            {.column = COL_CEILING, .slot = 6, .pattern = new Patterns::FadeFromRandom()},
            // {.column = COL_CEILING, .slot = 7, .pattern = new Patterns::SideWave()},
            // {.column = COL_CEILING, .slot = 8, .pattern = new Patterns::SinChase2Pattern()},
            {.column = COL_CEILING, .slot = 9, .pattern = new Patterns::GlowPulsePattern()},
            // {.column = COL_CEILING, .slot = 10, .pattern = new Patterns::SegmentChasePattern()},

            {.column = COL_FLASH, .slot = 0, .pattern = new Patterns::GlitchPattern()},
            {.column = COL_FLASH, .slot = 1, .pattern = new Patterns::PixelGlitchPattern()},
            // {.column = COL_FLASH, .slot = 2, .pattern = new Patterns::Lighthouse(&cceilingMap3dCombined)},
            // {.column = COL_FLASH, .slot = 3, .pattern = new Patterns::RadialGlitterFadePattern(&cceilingMap3dCombined)},
            // {.column = COL_FLASH, .slot = 4, .pattern = new Patterns::RadialFadePattern(&cceilingMap3dCombined)},
            {.column = COL_FLASH, .slot = 5, .pattern = new Patterns::LineLaunch(&ceilingMap3dCombined)},
            {.column = COL_FLASH, .slot = 6, .pattern = new Patterns::FadingNoisePattern()},
            {.column = COL_FLASH, .slot = 7, .pattern = new Patterns::SegmentGlitchPattern()},
            {.column = COL_FLASH, .slot = 8, .pattern = new Patterns::FlashesPattern()},
            {.column = COL_FLASH, .slot = 9, .pattern = new Patterns::StrobePattern()},
            {.column = COL_FLASH, .slot = 10, .pattern = new Patterns::StrobeHighlightPattern()},

            // {.column = COL_ARTNET, .slot = 0, .pattern = new Patterns::ArtNetPattern(0,16+48,60,ceilingMappedIndices)},


        });

    auto splitInput = new InputSlicer(
        input,
        {
            {0 * nbytes / 8, nbytes / 8, true},
            {1 * nbytes / 8, nbytes / 8, true},
            {2 * nbytes / 8, nbytes / 8, true},
            {3 * nbytes / 8, nbytes / 8, true},
            {0 * nbytes / 8, nbytes / 8, true},
            {1 * nbytes / 8, nbytes / 8, true},
            {2 * nbytes / 8, nbytes / 8, true},
            {3 * nbytes / 8, nbytes / 8, true},
            
            {0 * nbytes / 2, nbytes / 2, true},
            {1 * nbytes / 2, nbytes / 2, true},

            {0, nbytes, true},
        });

    for (int i = 0; i < 8; i++)
    {
        auto pipe = new ConvertPipe<RGBA, BGR>(
            splitInput->getInput(i),
            new UDPOutput("hyperslave6.local", 9611 + i, 45),
            columnsLut);
        hyp->addPipe(pipe);
    }

    auto splitMap = new PixelMapSplitter3d(&ceilingMap3dCombined, {nleds / 2, nleds / 2});
    for (int i = 0; i < 2; i++)
    {
        auto pipe = new ConvertPipe<RGBA, RGB>(
            splitInput->getInput(splitInput->size() -3 + i),
            new MonitorOutput3d(&hyp->webServer, splitMap->getMap(i)));
        hyp->addPipe(pipe);
    }

    auto pipe = new ConvertPipe<RGBA, RGB>(
        splitInput->getInput(splitInput->size()-1),
        new MonitorOutput(&hyp->webServer, &ceilingMap));
    hyp->addPipe(pipe);
}

void addColanderPipe(Hyperion *hyp)
{
    int nleds = ceilingMap3dCombined.size();
    int nbytes = nleds * sizeof(RGBA);

    auto input = new ControlHubInput<RGBA>(
        nleds,
        &hyp->hub,
        {
            // {.column = 1, .slot = i++, .pattern=new Patterns::Quadrants3d(&ceilingMap3dCombined)},

            // {.column = 1, .slot = i++, .pattern=new Patterns::Lighthouse(&cceilingMap3dCombined)},
            // {.column = 1, .slot = 4, .pattern=new Patterns::IndexMapTest()},
            // {.column = 1, .slot = 5, .pattern=new Patterns::CeilingChase()},

            {.column = COL_COLANDER, .slot = 0, .pattern = new Patterns::RibbenClivePattern<NegativeCosFast>()},
            {.column = COL_COLANDER, .slot = 1, .pattern = new Patterns::RibbenFlashPattern()},
            // {.column = COL_COLANDER, .slot = 2, .pattern = new Patterns::CeilingChase()},
            // {.column = COL_COLANDER, .slot = 3, .pattern = new Patterns::SinChasePattern()},
            // {.column = COL_COLANDER, .slot = 4, .pattern = new Patterns::SawChasePattern()},
            {.column = COL_COLANDER, .slot = 5, .pattern = new Patterns::FadeFromCenter()},
            {.column = COL_COLANDER, .slot = 6, .pattern = new Patterns::FadeFromRandom()},
            // {.column = COL_COLANDER, .slot = 7, .pattern = new Patterns::SideWave()},
            // {.column = COL_COLANDER, .slot = 8, .pattern = new Patterns::SinChase2Pattern()},
            {.column = COL_COLANDER, .slot = 9, .pattern = new Patterns::GlowPulsePattern()},
            // {.column = COL_COLANDER, .slot = 10, .pattern = new Patterns::SegmentChasePattern()},

            {.column = COL_FLASH, .slot = 0, .pattern = new Patterns::GlitchPattern()},
            {.column = COL_FLASH, .slot = 1, .pattern = new Patterns::PixelGlitchPattern()},
            // {.column = COL_FLASH, .slot = 5, .pattern = new Patterns::LineLaunch(&colanderMap3dCombined)},
            {.column = COL_FLASH, .slot = 6, .pattern = new Patterns::FadingNoisePattern()},
            {.column = COL_FLASH, .slot = 7, .pattern = new Patterns::SegmentGlitchPattern()},
            {.column = COL_FLASH, .slot = 8, .pattern = new Patterns::FlashesPattern()},
            {.column = COL_FLASH, .slot = 9, .pattern = new Patterns::StrobePattern()},
            {.column = COL_FLASH, .slot = 10, .pattern = new Patterns::StrobeHighlightPattern()},

            // {.column = COL_ARTNET, .slot = 0, .pattern = new Patterns::ArtNetPattern(0,16+48,60,ceilingMappedIndices)},


        });

    auto splitInput = new InputSlicer(
        input,
        {
            {0, nbytes, true},
            {0, nbytes, false},
            {0, nbytes, false},
        });


    auto pipe = new Pipe(
        splitInput->getInput(0),
        new UDPOutput("hyperslave6.local", 9611, 60));
    hyp->addPipe(pipe);

    hyp->addPipe(
        new ConvertPipe<RGBA, RGB>(
            splitInput->getInput(1),
            new MonitorOutput3d(&hyp->webServer, &colanderMap3dCombined, 60, 0.04)));
    
    hyp->addPipe(
        new ConvertPipe<RGBA, RGB>(
            splitInput->getInput(2),
            new MonitorOutput(&hyp->webServer, &colanderMap)));
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
