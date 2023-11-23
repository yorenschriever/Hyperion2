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
#include "core/generation/palettes.hpp"
#include "core/distribution/luts/colourCorrectionLut.hpp"
#include "core/generation/pixelMapSplitter3d.hpp"
#include "patterns/ceiling.hpp"
#include "patterns/common.hpp"
#include "patterns/window.hpp"
#include "patterns/chandelier.hpp"
#include "patterns/mask.hpp"
#include "patterns/debug.hpp"
#include "setViewParams.hpp"
#include <vector>
#include "artNetPattern.hpp"
#include "artNetController.hpp"
#include "midiController.hpp"
#include "patterns/videoPattern.hpp"

#define WITHVIDEO 0

#if WITHVIDEO
#include "videos/processed/Kapel_Cirkels_1.hpp"
#include "videos/processed/Kapel_Cirkels_2.hpp"
#include "videos/processed/Kapel_Cirkels_3.hpp"
#include "videos/processed/Kapel_Cirkels_4.hpp"
#include "videos/processed/Kapel_Cirkels_5.hpp"
#include "videos/processed/Kapel_Cirkels_6.hpp"
#include "videos/processed/Kapel_Cirkels_7.hpp"
#include "videos/processed/Kapel_Grid_1.hpp"
#include "videos/processed/Kapel_Grid_2.hpp"
#include "videos/processed/Kapel_Grid_3.hpp"
#include "videos/processed/Kapel_Grid_4.hpp"
#include "videos/processed/Kapel_Heen_en_Weer_1.hpp"
#include "videos/processed/Kapel_Heen_en_Weer_2.hpp"
#include "videos/processed/Kapel_Heen_en_Weer_3.hpp"
#include "videos/processed/Kapel_Noise_1.hpp"
#include "videos/processed/Kapel_Noise_2.hpp"
#include "videos/processed/Kapel_Squares_1.hpp"
#include "videos/processed/Kapel_Triangle_1.hpp"
#include "videos/processed/Kapel_Wave_1.hpp"
#include "videos/processed/Kapel_Wave_2.hpp"
#include "videos/processed/Kapel_Wave_3.hpp"
#include "videos/processed/Kapel_Wave_4.hpp"
#include "videos/processed/Kapel_XX_1.hpp"
#include "videos/processed/Kapel_XX_2.hpp"
#include "videos/processed/Kapel_XX_3.hpp"
#endif

//2.7 gamma experimentally determined in dark environment
LUT *columnsLut = new ColourCorrectionLUT(2.7, 255, 255, 255, 255);

auto cwindowMap3dCombined = windowMap3dCombined.toCylindricalXZ(0, chandelierYOffset);
auto cchandelierMap3dCombined = chandelierMap3dCombined.toCylindricalXZ(0, chandelierYOffset);
auto cceilingMap3dCombined = ceilingMap3dCombined.toCylindricalXZ(0, chandelierYOffset);
auto cchandelierMap3d = chandelierMap3d.toCylindricalXZ();

void addPaletteColumn(Hyperion *hyp);

void addWindowPipe(Hyperion *);
void addChandelierPipe(Hyperion *);
void addCeilingPipe(Hyperion *);

#define COL_PALETTE 0
#define COL_WINDOW 1
#define COL_CHANDELIER 2
#define COL_CEILING 3
#define COL_ALL 4
#define COL_VIDEO 5
#define COL_MASK 6
#define COL_FLASH 7
#define COL_DEBUG 8
#define COL_ARTNET 9

typedef struct
{
    const char *host;
    const unsigned short port;
} Slave;

int main()
{
    auto hyp = new Hyperion();
    hyp->midiControllerFactory = new CustomMidiControllerFactory();

    // hyp->hub.addParams(new Params("second params"));

    addPaletteColumn(hyp);

    addWindowPipe(hyp);
    addChandelierPipe(hyp);
    addCeilingPipe(hyp);

    // Tempo::AddSource(new ConstantTempo(120));

    // select first palette
    hyp->hub.buttonPressed(0, 0);
    hyp->hub.setFlashColumn(0, false, true);
    hyp->hub.setForcedSelection(0);

    //hyp->hub.buttonPressed(1, 0);
    hyp->hub.setFlashColumn(COL_FLASH, true, false);
    hyp->hub.buttonPressed(COL_ARTNET, 0);

    hyp->hub.setColumnName(COL_PALETTE, "Palette");
    hyp->hub.setColumnName(COL_WINDOW, "Windows");
    hyp->hub.setColumnName(COL_CHANDELIER, "Chandelier");
    hyp->hub.setColumnName(COL_CEILING, "Ceiling");
    hyp->hub.setColumnName(COL_ALL, "All");
    hyp->hub.setColumnName(COL_MASK, "Mask");
    hyp->hub.setColumnName(COL_FLASH, "Flash");
    hyp->hub.setColumnName(COL_ARTNET, "ArtNet");
    hyp->hub.setColumnName(COL_DEBUG, "Debug");
    hyp->hub.setColumnName(COL_VIDEO, "Video");

    hyp->start();
    setViewParams(hyp);

    auto artNetController = new ArtNetController();
    artNetController->subscribe(&hyp->hub,1);
    artNetController->linkToPalette(&grandMA);

    while (1)
        Thread::sleep(60 * 1000);
}

void addWindowPipe(Hyperion *hyp)
{
    int nleds = windowMap3dCombined.size();
    int nbytes = nleds * sizeof(RGBA);
    int segmentSize = nleds / 8;

    auto input = new ControlHubInput<RGBA>(
        windowMap3dCombined.size(),
        &hyp->hub,
        {
            // {.column = 1, .slot = i++, .pattern=new Patterns::Quadrants3d(&windowMap3dCombinedLeft)},

            {.column = COL_WINDOW, .slot = 0, .pattern = new Patterns::GrowShrink(&windowMap3dCombined)},
            {.column = COL_WINDOW, .slot = 1, .pattern = new Patterns::VerticallyIsolated(&windowMap3dCombined)},
            {.column = COL_WINDOW, .slot = 2, .pattern = new Patterns::RotatingRingsPattern(&windowMap3dCombined)},
            {.column = COL_WINDOW, .slot = 3, .pattern = new Patterns::OnBeatWindowChaseUpPattern(&windowMap3dCombined)},
            {.column = COL_WINDOW, .slot = 4, .pattern = new Patterns::HorizontalSin(&windowMap3dCombined)},
            {.column = COL_WINDOW, .slot = 5, .pattern = new Patterns::OnbeatFadePattern()},
            {.column = COL_WINDOW, .slot = 6, .pattern = new Patterns::WindowGlitchPattern()},
            {.column = COL_WINDOW, .slot = 7, .pattern = new Patterns::GlowPulsePattern()},
            {.column = COL_WINDOW, .slot = 8, .pattern = new Patterns::RibbenClivePattern<NegativeCosFast>(segmentSize)},
            {.column = COL_WINDOW, .slot = 9, .pattern = new Patterns::RibbenFlashPattern(segmentSize)},

            {.column = COL_ALL, .slot = 0, .pattern = new Patterns::GlitchPattern(segmentSize)},
            {.column = COL_ALL, .slot = 1, .pattern = new Patterns::PixelGlitchPattern()},
            {.column = COL_ALL, .slot = 2, .pattern = new Patterns::Lighthouse(&cwindowMap3dCombined)},
            {.column = COL_ALL, .slot = 3, .pattern = new Patterns::RadialGlitterFadePattern(&cwindowMap3dCombined)},
            {.column = COL_ALL, .slot = 4, .pattern = new Patterns::LineLaunch(&windowMap3dCombined)},
            {.column = COL_ALL, .slot = 5, .pattern = new Patterns::SegmentGlitchPattern()},
            {.column = COL_ALL, .slot = 6, .pattern = new Patterns::FlashesPattern()},
            {.column = COL_ALL, .slot = 7, .pattern = new Patterns::StrobePattern()},
            {.column = COL_ALL, .slot = 8, .pattern = new Patterns::StrobeHighlightPattern()},

            {.column = COL_ARTNET, .slot = 0, .pattern = new Patterns::ArtNetPatternRGB(0,(48+48)*4,segmentSize)},
            // {.column = COL_ARTNET, .slot = 1, .pattern = new Patterns::ArtNetPattern(0,48+48,segmentSize)},

            {.column = COL_MASK, .slot = 3, .pattern = new Patterns::RotatingRingsMaskPattern(&windowMap3dCombined)},
            {.column = COL_MASK, .slot = 4, .pattern = new Patterns::GlowPulseMaskPattern()},

            {.column = COL_FLASH, .slot = 0, .pattern = new Patterns::GlitchPattern(segmentSize)},
            {.column = COL_FLASH, .slot = 1, .pattern = new Patterns::PixelGlitchPattern()},
            {.column = COL_FLASH, .slot = 2, .pattern = new Patterns::LineLaunch(&windowMap3dCombined)},
            {.column = COL_FLASH, .slot = 3, .pattern = new Patterns::FadingNoisePattern()},
            {.column = COL_FLASH, .slot = 4, .pattern = new Patterns::SegmentGlitchPattern()},
            {.column = COL_FLASH, .slot = 5, .pattern = new Patterns::FlashesPattern()},
            {.column = COL_FLASH, .slot = 6, .pattern = new Patterns::StrobePattern()},
            {.column = COL_FLASH, .slot = 7, .pattern = new Patterns::StrobeHighlightPattern()},
            {.column = COL_FLASH, .slot = 8, .pattern = new Patterns::RadialGlitterFadePattern(&cwindowMap3dCombined)},

            {.column = COL_DEBUG, .slot = 0, .pattern = new Patterns::ShowStarts(1,segmentSize)},
            {.column = COL_DEBUG, .slot = 1, .pattern = new Patterns::OneColor(RGB(255, 0, 0), "Red")},
            {.column = COL_DEBUG, .slot = 2, .pattern = new Patterns::OneColor(RGB(0, 255, 0), "Green")},
            {.column = COL_DEBUG, .slot = 3, .pattern = new Patterns::OneColor(RGB(0, 0, 255), "Blue")},
            {.column = COL_DEBUG, .slot = 4, .pattern = new Patterns::OneColor(RGB(255, 255, 255), "White")},
            {.column = COL_DEBUG, .slot = 5, .pattern = new Patterns::OneColor(RGB(127, 127, 127), "White 50%")},
            {.column = COL_DEBUG, .slot = 7, .pattern = new Patterns::PaletteColors()},
            {.column = COL_DEBUG, .slot = 8, .pattern = new Patterns::WindowMapPattern()},
        });

    auto splitInput = new InputSlicer(
        input,
        {
            {0 * nbytes / 8, nbytes / 8, true},
            {1 * nbytes / 8, nbytes / 8, true},
            {2 * nbytes / 8, nbytes / 8, true},
            {3 * nbytes / 8, nbytes / 8, true},
            {4 * nbytes / 8, nbytes / 8, true},
            {5 * nbytes / 8, nbytes / 8, true},
            {6 * nbytes / 8, nbytes / 8, true},
            {7 * nbytes / 8, nbytes / 8, true},

            {0, nbytes / 2, false},
            {nbytes / 2, nbytes / 2, false},
        });

    auto splitMap = new PixelMapSplitter3d(&windowMap3dCombined, {nleds / 2, nleds / 2});

    Slave slaves[8] = {
        {.host = "hyperslave1.local", .port = 9611},
        {.host = "hyperslave1.local", .port = 9612},
        {.host = "hyperslave1.local", .port = 9613},
        {.host = "hyperslave1.local", .port = 9614},

        // {.host = "hyperslave1.local", .port = 9615},
        // {.host = "hyperslave1.local", .port = 9616},
        // {.host = "hyperslave1.local", .port = 9617},
        // {.host = "hyperslave1.local", .port = 9618},

        {.host = "hyperslave2.local", .port = 9611},
        {.host = "hyperslave2.local", .port = 9612},
        {.host = "hyperslave2.local", .port = 9613},
        {.host = "hyperslave2.local", .port = 9614},

        // {.host = "hyperslave2.local", .port = 9615},
        // {.host = "hyperslave2.local", .port = 9616},
        // {.host = "hyperslave2.local", .port = 9617},
        // {.host = "hyperslave2.local", .port = 9618},
    };

    for (int i = 0; i < splitInput->size() - 2; i++)
    {
        auto pipe = new ConvertPipe<RGBA, GBR>(
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

    // hyp->addPipe(
    //     new ConvertPipe<RGBA, RGB>(
    //         splitInput->getInput(splitInput->size() - 2),
    //         new MonitorOutput(&hyp->webServer, &windowMap)));
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

            {.column = COL_CHANDELIER, .slot = 0, .pattern = new Patterns::BreathingGradientPattern(&cchandelierMap3d)},
            {.column = COL_CHANDELIER, .slot = 1, .pattern = new Patterns::GlowPulsePattern()},
            {.column = COL_CHANDELIER, .slot = 2, .pattern = new Patterns::SegmentChasePattern()},
            {.column = COL_CHANDELIER, .slot = 3, .pattern = new Patterns::Lighthouse(&cchandelierMap3d)},
            {.column = COL_CHANDELIER, .slot = 4, .pattern = new Patterns::BarLFO()},
            {.column = COL_CHANDELIER, .slot = 5, .pattern = new Patterns::GradientLFO()},
            {.column = COL_CHANDELIER, .slot = 6, .pattern = new Patterns::FadeFromRandomChandelier()},
            {.column = COL_CHANDELIER, .slot = 7, .pattern = new Patterns::RibbenClivePattern<NegativeCosFast>()},
            {.column = COL_CHANDELIER, .slot = 8, .pattern = new Patterns::RibbenFlashPattern()},
            {.column = COL_CHANDELIER, .slot = 9, .pattern = new Patterns::StaticGradientPattern(&cchandelierMap3d)},
            
            {.column = COL_ALL, .slot = 0, .pattern = new Patterns::GlitchPattern()},
            {.column = COL_ALL, .slot = 1, .pattern = new Patterns::PixelGlitchPattern()},
            {.column = COL_ALL, .slot = 2, .pattern = new Patterns::Lighthouse(&cchandelierMap3dCombined)},
            {.column = COL_ALL, .slot = 3, .pattern = new Patterns::RadialGlitterFadePattern(&cchandelierMap3dCombined)},
            {.column = COL_ALL, .slot = 4, .pattern = new Patterns::LineLaunch(&chandelierMap3dCombined)},
            {.column = COL_ALL, .slot = 5, .pattern = new Patterns::SegmentGlitchPattern()},
            {.column = COL_ALL, .slot = 6, .pattern = new Patterns::FlashesPattern()},
            {.column = COL_ALL, .slot = 7, .pattern = new Patterns::StrobePattern()},
            {.column = COL_ALL, .slot = 8, .pattern = new Patterns::StrobeHighlightPattern()},

            {.column = COL_ARTNET, .slot = 0, .pattern = new Patterns::ArtNetPatternRGB(0,48*4)},
            // {.column = COL_ARTNET, .slot = 1, .pattern = new Patterns::ArtNetPattern(0,48)},

            {.column = COL_MASK, .slot = 5, .pattern = new Patterns::SinChaseMaskPattern()},
            {.column = COL_MASK, .slot = 6, .pattern = new Patterns::GlowPulseMaskPattern()},
            {.column = COL_MASK, .slot = 7, .pattern = new Patterns::SideWaveMask(nullptr)}, /*ceilingMappedIndices*/

            {.column = COL_FLASH, .slot = 0, .pattern = new Patterns::GlitchPattern()},
            {.column = COL_FLASH, .slot = 1, .pattern = new Patterns::PixelGlitchPattern()},
            {.column = COL_FLASH, .slot = 2, .pattern = new Patterns::LineLaunch(&chandelierMap3dCombined)},
            {.column = COL_FLASH, .slot = 3, .pattern = new Patterns::FadingNoisePattern()},
            {.column = COL_FLASH, .slot = 4, .pattern = new Patterns::SegmentGlitchPattern()},
            {.column = COL_FLASH, .slot = 5, .pattern = new Patterns::FlashesPattern()},
            {.column = COL_FLASH, .slot = 6, .pattern = new Patterns::StrobePattern()},
            {.column = COL_FLASH, .slot = 7, .pattern = new Patterns::StrobeHighlightPattern()},
            {.column = COL_FLASH, .slot = 8, .pattern = new Patterns::RadialGlitterFadePattern(&cchandelierMap3dCombined)},

            {.column = COL_DEBUG, .slot = 0, .pattern = new Patterns::ShowStarts(nleds/60/8)},
            {.column = COL_DEBUG, .slot = 1, .pattern = new Patterns::OneColor(RGB(255, 0, 0), "Red")},
            {.column = COL_DEBUG, .slot = 2, .pattern = new Patterns::OneColor(RGB(0, 255, 0), "Green")},
            {.column = COL_DEBUG, .slot = 3, .pattern = new Patterns::OneColor(RGB(0, 0, 255), "Blue")},
            {.column = COL_DEBUG, .slot = 4, .pattern = new Patterns::OneColor(RGB(255, 255, 255), "White")},
            {.column = COL_DEBUG, .slot = 5, .pattern = new Patterns::OneColor(RGB(127, 127, 127), "White 50%")},
            {.column = COL_DEBUG, .slot = 9, .pattern = new Patterns::Gamma(60)},
        });

    auto splitInput = new InputSlicer(
        input,
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
            new UDPOutput("hyperslave4.local", 9611 + i, 60),
            columnsLut);
        hyp->addPipe(pipe);
    }

    hyp->addPipe(
        new ConvertPipe<RGBA, RGB>(
            splitInput->getInput(splitInput->size() - 1),
            new MonitorOutput3d(&hyp->webServer, &chandelierMap3dCombined)));
            // new MonitorOutput3d(&hyp->webServer, &chandelierMap3d, 60, 0.03)));
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

            {.column = COL_CEILING, .slot = 0, .pattern = new Patterns::SinChasePattern()},
            {.column = COL_CEILING, .slot = 1, .pattern = new Patterns::SawChasePattern()},
            {.column = COL_CEILING, .slot = 2, .pattern = new Patterns::FadeFromCenter()},
            {.column = COL_CEILING, .slot = 3, .pattern = new Patterns::FadeFromRandom()},
            {.column = COL_CEILING, .slot = 4, .pattern = new Patterns::SideWave()},
            {.column = COL_CEILING, .slot = 5, .pattern = new Patterns::SinChase2Pattern()},
            {.column = COL_CEILING, .slot = 6, .pattern = new Patterns::GlowPulsePattern()},
            {.column = COL_CEILING, .slot = 7, .pattern = new Patterns::SegmentChasePattern()},
            {.column = COL_CEILING, .slot = 8, .pattern = new Patterns::RibbenClivePattern<NegativeCosFast>()},
            {.column = COL_CEILING, .slot = 9, .pattern = new Patterns::RibbenFlashPattern()},
            {.column = COL_CEILING, .slot = 10, .pattern = new Patterns::CeilingChase()},

#if WITHVIDEO
            {.column = COL_VIDEO, .slot = 0, .pattern = new VideoPalettePattern(&anim_Kapel_Triangle_1,"Triangle 1")},
            {.column = COL_VIDEO, .slot = 1, .pattern = new VideoPalettePattern(&anim_Kapel_Wave_1,"Wave 1")},
            {.column = COL_VIDEO, .slot = 2, .pattern = new VideoPalettePattern(&anim_Kapel_Wave_2,"Wave 2")},
            {.column = COL_VIDEO, .slot = 3, .pattern = new VideoPalettePattern(&anim_Kapel_Wave_3,"Wave 3")},
            {.column = COL_VIDEO, .slot = 4, .pattern = new VideoPalettePattern(&anim_Kapel_Wave_4,"Wave 4")},
            {.column = COL_VIDEO, .slot = 5, .pattern = new VideoPalettePattern(&anim_Kapel_XX_1,"XX 1")},
            {.column = COL_VIDEO, .slot = 6, .pattern = new VideoPalettePattern(&anim_Kapel_XX_2,"XX 2")},
            {.column = COL_VIDEO, .slot = 7, .pattern = new VideoPalettePattern(&anim_Kapel_XX_3,"XX 3")},
            {.column = COL_VIDEO, .slot = 8, .pattern = new VideoPalettePattern(&anim_Kapel_Cirkels_1,"Cirkels 1")},
            {.column = COL_VIDEO, .slot = 9, .pattern = new VideoPalettePattern(&anim_Kapel_Cirkels_3,"Cirkels 3")},
            {.column = COL_VIDEO, .slot = 10, .pattern = new VideoPalettePattern(&anim_Kapel_Cirkels_4,"Cirkels 4")},
            {.column = COL_VIDEO, .slot = 11, .pattern = new VideoPalettePattern(&anim_Kapel_Cirkels_5,"Cirkels 5")},
            {.column = COL_VIDEO, .slot = 12, .pattern = new VideoPalettePattern(&anim_Kapel_Cirkels_6,"Cirkels 6")},
            {.column = COL_VIDEO, .slot = 13, .pattern = new VideoPalettePattern(&anim_Kapel_Cirkels_7,"Cirkels 7")},
            {.column = COL_VIDEO, .slot = 14, .pattern = new VideoPalettePattern(&anim_Kapel_Heen_en_Weer_1,"Heen_en_Weer 1")},
            {.column = COL_VIDEO, .slot = 15, .pattern = new VideoPalettePattern(&anim_Kapel_Heen_en_Weer_2,"Heen_en_Weer 2")},
            {.column = COL_VIDEO, .slot = 16, .pattern = new VideoPalettePattern(&anim_Kapel_Heen_en_Weer_3,"Heen_en_Weer 3")},
            {.column = COL_VIDEO, .slot = 17, .pattern = new VideoPalettePattern(&anim_Kapel_Noise_1,"Noise 1")},
            {.column = COL_VIDEO, .slot = 18, .pattern = new VideoPalettePattern(&anim_Kapel_Noise_2,"Noise 2")},
            {.column = COL_VIDEO, .slot = 19, .pattern = new VideoPalettePattern(&anim_Kapel_Squares_1,"Squares 1")},
            {.column = COL_VIDEO, .slot = 20, .pattern = new VideoPalettePattern(&anim_Kapel_Grid_1,"Grid 1")},
            {.column = COL_VIDEO, .slot = 21, .pattern = new VideoPalettePattern(&anim_Kapel_Grid_2,"Grid 2")},
            {.column = COL_VIDEO, .slot = 22, .pattern = new VideoPalettePattern(&anim_Kapel_Grid_3,"Grid 3")},
            {.column = COL_VIDEO, .slot = 23, .pattern = new VideoPalettePattern(&anim_Kapel_Grid_4,"Grid 4")},
#endif 

            {.column = COL_ALL, .slot = 0, .pattern = new Patterns::GlitchPattern()},
            {.column = COL_ALL, .slot = 1, .pattern = new Patterns::PixelGlitchPattern()},
            {.column = COL_ALL, .slot = 2, .pattern = new Patterns::Lighthouse(&cceilingMap3dCombined)},
            {.column = COL_ALL, .slot = 3, .pattern = new Patterns::RadialGlitterFadePattern(&cceilingMap3dCombined)},
            {.column = COL_ALL, .slot = 4, .pattern = new Patterns::LineLaunch(&ceilingMap3dCombined)},
            {.column = COL_ALL, .slot = 5, .pattern = new Patterns::SegmentGlitchPattern()},
            {.column = COL_ALL, .slot = 6, .pattern = new Patterns::FlashesPattern()},
            {.column = COL_ALL, .slot = 7, .pattern = new Patterns::StrobePattern()},
            {.column = COL_ALL, .slot = 8, .pattern = new Patterns::StrobeHighlightPattern()},

            {.column = COL_ARTNET, .slot = 0, .pattern = new Patterns::ArtNetPatternRGB(0,0,60,ceilingMappedIndices)},
            // {.column = COL_ARTNET, .slot = 1, .pattern = new Patterns::ArtNetPattern(0,0,60,ceilingMappedIndices)},

            {.column = COL_MASK, .slot = 0, .pattern = new Patterns::GlowPulseMaskPattern()},
            {.column = COL_MASK, .slot = 1, .pattern = new Patterns::SinChaseMaskPattern()},
            {.column = COL_MASK, .slot = 2, .pattern = new Patterns::SideWaveMask(ceilingMappedIndices)},

            {.column = COL_FLASH, .slot = 0, .pattern = new Patterns::GlitchPattern()},
            {.column = COL_FLASH, .slot = 1, .pattern = new Patterns::PixelGlitchPattern()},
            {.column = COL_FLASH, .slot = 2, .pattern = new Patterns::LineLaunch(&ceilingMap3dCombined)},
            {.column = COL_FLASH, .slot = 3, .pattern = new Patterns::FadingNoisePattern()},
            {.column = COL_FLASH, .slot = 4, .pattern = new Patterns::SegmentGlitchPattern()},
            {.column = COL_FLASH, .slot = 5, .pattern = new Patterns::FlashesPattern()},
            {.column = COL_FLASH, .slot = 6, .pattern = new Patterns::StrobePattern()},
            {.column = COL_FLASH, .slot = 7, .pattern = new Patterns::StrobeHighlightPattern()},
            {.column = COL_FLASH, .slot = 8, .pattern = new Patterns::RadialGlitterFadePattern(&cceilingMap3dCombined)},

            {.column = COL_DEBUG, .slot = 0, .pattern = new Patterns::ShowStarts(12)},
            {.column = COL_DEBUG, .slot = 1, .pattern = new Patterns::OneColor(RGB(255, 0, 0), "Red")},
            {.column = COL_DEBUG, .slot = 2, .pattern = new Patterns::OneColor(RGB(0, 255, 0), "Green")},
            {.column = COL_DEBUG, .slot = 3, .pattern = new Patterns::OneColor(RGB(0, 0, 255), "Blue")},
            {.column = COL_DEBUG, .slot = 4, .pattern = new Patterns::OneColor(RGB(255, 255, 255), "White")},
            {.column = COL_DEBUG, .slot = 5, .pattern = new Patterns::OneColor(RGB(127, 127, 127), "White 50%")},
            {.column = COL_DEBUG, .slot = 6, .pattern = new Patterns::PaletteGradient(ceilingMappedIndices)},
            {.column = COL_DEBUG, .slot = 9, .pattern = new Patterns::Gamma(60)},
        });

    auto splitInput = new InputSlicer(
        input,
        {
            {0 * nbytes / 4, nbytes / 4, true},
            {1 * nbytes / 4, nbytes / 4, true},
            {2 * nbytes / 4, nbytes / 4, true},
            {3 * nbytes / 4, nbytes / 4, true},
            {0, nbytes, false},
        });

    for (int i = 0; i < splitInput->size() - 1; i++)
    {
        auto pipe = new ConvertPipe<RGBA, GBR>(
            splitInput->getInput(i),
            new UDPOutput("hyperslave3.local", 9611 + i, 45),
            columnsLut);
        hyp->addPipe(pipe);
    }

    hyp->addPipe(
        new ConvertPipe<RGBA, RGB>(
            splitInput->getInput(splitInput->size() - 1),
            new MonitorOutput3d(&hyp->webServer, &ceilingMap3dCombined)));
            //new MonitorOutput(&hyp->webServer, &ceilingMap)));
}

void addPaletteColumn(Hyperion *hyp)
{
    auto paletteColumn = new PaletteColumn(
        &hyp->hub,
        0,
        0,
        {
            &grandMA,
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
