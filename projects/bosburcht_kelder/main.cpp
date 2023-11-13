#include "core/distribution/inputs/inputSlicer.hpp"
#include "core/distribution/luts/colourCorrectionLut.hpp"
#include "core/distribution/luts/incandescentLut.hpp"
#include "core/distribution/outputs/neopixelOutput.hpp"
#include "core/distribution/outputs/spiOutput.hpp"
#include "core/generation/pixelMapSplitter3d.hpp"
#include "core/hyperion.hpp"
#include "distribution/outputs/cloneOutput.hpp"
#include "mapping/ceilingMap.hpp"
#include "mapping/ceilingMap3dCombined.hpp"
#include "mapping/colanderMap.hpp"
#include "mapping/colanderMap3dCombined.hpp"
#include "core/generation/palettes.hpp"
#include "patterns/ceiling.hpp"
#include "patterns/colander.hpp"
#include "patterns/window.hpp"
#include "patterns/common.hpp"
#include "patterns/mask.hpp"
#include "patterns/videoPattern.hpp"
#include "animation.hpp"
#include "setViewParams.hpp"
#include <vector>
#include "videos/Gradient_circle.hpp"
#include "videos/driehoek_v1.hpp"
#include "videos/processed/Heen_en_weer_1.hpp"
#include "videos/processed/Heen_en_weer_2.hpp"
#include "videos/processed/Heen_en_weer_3.hpp"
#include "videos/processed/Heen_en_weer_4.hpp"
#include "videos/processed/Noise_1.hpp"
#include "videos/processed/Noise_2.hpp"
#include "videos/processed/Noise_3.hpp"
#include "videos/processed/Schuine_lijnen_1.hpp"
#include "videos/processed/Spiraal_1.hpp"
#include "videos/processed/Spiraal_2.hpp"
#include "videos/processed/Spiraal_3.hpp"
#include "videos/processed/Spiraal_4.hpp"
#include "videos/processed/Wipe_1.hpp"
#include "videos/processed/Wipe_2.hpp"
#include "videos/processed/Wipe_3.hpp"
#include "videos/processed/Wipe_4.hpp"
#include "videos/processed/Wipe_5.hpp"
#include "videos/processed/X_1.hpp"
#include "videos/processed/X_2.hpp"
#include "videos/processed/X_3.hpp"

LUT *columnsLut = nullptr; // new ColourCorrectionLUT(1, 255, 200, 200, 200);
LUT *incandescentLut8 = new IncandescentLUT(2.5, 255, 24);

void addPaletteColumn(Hyperion *hyp);

void addColanderPipe(Hyperion *);
void addCeilingPipe(Hyperion *);

#define COL_PALETTE 0
#define COL_COLANDER1 1
#define COL_COLANDER2 2
#define COL_CEILING1 3
#define COL_CEILING2 4
#define COL_VIDEO 5
#define COL_MASK 6
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
    hyp->hub.buttonPressed(COL_PALETTE, 1);
    hyp->hub.setFlashColumn(COL_PALETTE, false, true);
    hyp->hub.setForcedSelection(COL_PALETTE);

    hyp->hub.setFlashColumn(COL_FLASH, true, false);

    // hyp->hub.buttonPressed(COL_CEILING, 0);

    hyp->hub.setColumnName(COL_PALETTE, "Palette");
    hyp->hub.setColumnName(COL_COLANDER1, "Vergiet");
    hyp->hub.setColumnName(COL_COLANDER2, "Vergiet");
    hyp->hub.setColumnName(COL_CEILING1, "Ceiling");
    hyp->hub.setColumnName(COL_CEILING2, "Ceiling");
    hyp->hub.setColumnName(COL_VIDEO, "Video");
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
            {.column = COL_CEILING1, .slot = 0, .pattern = new Ceiling::Fireworks()},
            {.column = COL_CEILING1, .slot = 1, .pattern = new Ceiling::Chaser()},
            {.column = COL_CEILING1, .slot = 2, .pattern = new Ceiling::RibbenClivePattern<NegativeCosFast>(20)},
            {.column = COL_CEILING1, .slot = 3, .pattern = new Ceiling::RibbenFlashPattern()},
            {.column = COL_CEILING1, .slot = 4, .pattern = new Patterns::FadeFromCenter()},
            {.column = COL_CEILING1, .slot = 5, .pattern = new Patterns::FadeFromRandom()},
            {.column = COL_CEILING1, .slot = 6, .pattern = new Patterns::FadeFromRandom(600)},

            {.column = COL_CEILING2, .slot = 0, .pattern = new Patterns::GlowPulsePattern()},
            {.column = COL_CEILING2, .slot = 1, .pattern = new Patterns::SegmentChasePattern(600)},
            {.column = COL_CEILING2, .slot = 2, .pattern = new Patterns::GlitchPattern()},
            {.column = COL_CEILING2, .slot = 3, .pattern = new Patterns::PixelGlitchPattern()},
            {.column = COL_CEILING2, .slot = 4, .pattern = new Patterns::LineLaunch(&ceilingMap3dCombined)},
            {.column = COL_CEILING2, .slot = 5, .pattern = new Patterns::SegmentGlitchPattern()},
            {.column = COL_CEILING2, .slot = 6, .pattern = new Patterns::FlashesPattern()},
            {.column = COL_CEILING2, .slot = 7, .pattern = new Patterns::StrobePattern()},

            // {.column = COL_VIDEO, .slot = 0, .pattern = new VideoPattern(&anim_driehoek_v1)},
            // {.column = COL_VIDEO, .slot = 1, .pattern = new VideoPalettePattern(&anim_driehoek_v1)},
            // {.column = COL_VIDEO, .slot = 2, .pattern = new VideoPalettePattern(&anim_Noise_3)},

            {.column = COL_VIDEO, .slot = 0 , .pattern = new VideoPalettePattern(&anim_Heen_en_weer_1,"Heen en weer 1")},
            {.column = COL_VIDEO, .slot = 1 , .pattern = new VideoPalettePattern(&anim_Heen_en_weer_2,"Heen en weer 2")},
            {.column = COL_VIDEO, .slot = 2 , .pattern = new VideoPalettePattern(&anim_Heen_en_weer_3,"Heen en weer 3")},
            {.column = COL_VIDEO, .slot = 3 , .pattern = new VideoPalettePattern(&anim_Heen_en_weer_4,"Heen en weer 4")},
            {.column = COL_VIDEO, .slot = 4 , .pattern = new VideoPalettePattern(&anim_Noise_1,"Noise 1")},
            {.column = COL_VIDEO, .slot = 5 , .pattern = new VideoPalettePattern(&anim_Noise_2,"Noise 2")},
            {.column = COL_VIDEO, .slot = 6 , .pattern = new VideoPalettePattern(&anim_Noise_3,"Noise 3")},
            {.column = COL_VIDEO, .slot = 7 , .pattern = new VideoPalettePattern(&anim_Schuine_lijnen_1,"Schuine lijnen 1")},
            {.column = COL_VIDEO, .slot = 8 , .pattern = new VideoPalettePattern(&anim_Spiraal_1,"Spiraal 1")},
            {.column = COL_VIDEO, .slot = 9 , .pattern = new VideoPalettePattern(&anim_Spiraal_2,"Spiraal 2")},
            {.column = COL_VIDEO, .slot = 10, .pattern = new VideoPalettePattern(&anim_Spiraal_3,"Spiraal 3")},
            {.column = COL_VIDEO, .slot = 11, .pattern = new VideoPalettePattern(&anim_Spiraal_4,"Spiraal 4")},
            {.column = COL_VIDEO, .slot = 12, .pattern = new VideoPalettePattern(&anim_Wipe_1,"Wipe 1")},
            {.column = COL_VIDEO, .slot = 13, .pattern = new VideoPalettePattern(&anim_Wipe_2,"Wipe 2")},
            {.column = COL_VIDEO, .slot = 14, .pattern = new VideoPalettePattern(&anim_Wipe_3,"Wipe 3")},
            {.column = COL_VIDEO, .slot = 15, .pattern = new VideoPalettePattern(&anim_Wipe_4,"Wipe 4")},
            {.column = COL_VIDEO, .slot = 16, .pattern = new VideoPalettePattern(&anim_Wipe_5,"Wipe 5")},
            {.column = COL_VIDEO, .slot = 17, .pattern = new VideoPalettePattern(&anim_X_1,"X 1")},
            {.column = COL_VIDEO, .slot = 18, .pattern = new VideoPalettePattern(&anim_X_2,"X 2")},
            {.column = COL_VIDEO, .slot = 19, .pattern = new VideoPalettePattern(&anim_X_3,"X 3")},

            {.column = COL_MASK, .slot = 0, .pattern = new Patterns::GlowPulseMaskPattern()},
            {.column = COL_MASK, .slot = 1, .pattern = new Patterns::SinChaseMaskPattern()},

            {.column = COL_FLASH, .slot = 0, .pattern = new Patterns::GlitchPattern()},
            {.column = COL_FLASH, .slot = 1, .pattern = new Patterns::PixelGlitchPattern()},
            {.column = COL_FLASH, .slot = 2, .pattern = new Patterns::LineLaunch(&ceilingMap3dCombined)},
            {.column = COL_FLASH, .slot = 3, .pattern = new Patterns::FadingNoisePattern()},
            {.column = COL_FLASH, .slot = 4, .pattern = new Patterns::SegmentGlitchPattern()},
            {.column = COL_FLASH, .slot = 5, .pattern = new Patterns::FlashesPattern()},
            {.column = COL_FLASH, .slot = 6, .pattern = new Patterns::StrobePattern()},
            {.column = COL_FLASH, .slot = 7, .pattern = new Patterns::StrobeHighlightPattern()},
            {.column = COL_FLASH, .slot = 8, .pattern = new Patterns::WindowGlitchPattern()},



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

            {0 * nbytes / 2, nbytes / 2, false},
            {1 * nbytes / 2, nbytes / 2, false},

            {0, nbytes, false},
        });

    for (int i = 0; i < 8; i++)
    {
        auto pipe = new ConvertPipe<RGBA, BGR>(
            splitInput->getInput(i),
            new UDPOutput("hyperslave5.local", 9611 + i, 45),
            columnsLut);
        hyp->addPipe(pipe);
    }

    auto splitMap = new PixelMapSplitter3d(&ceilingMap3dCombined, {nleds / 2, nleds / 2});
    for (int i = 0; i < 2; i++)
    {
        auto pipe = new ConvertPipe<RGBA, RGB>(
            splitInput->getInput(splitInput->size() - 3 + i),
            new MonitorOutput3d(&hyp->webServer, splitMap->getMap(i)));
        hyp->addPipe(pipe);
    }

    // auto pipe = new ConvertPipe<RGBA, RGB>(
    //     splitInput->getInput(splitInput->size() - 1),
    //     new MonitorOutput(&hyp->webServer, &ceilingMap));
    // hyp->addPipe(pipe);
}

void addColanderPipe(Hyperion *hyp)
{
    int nleds = colanderMap.size();
    int nbytes = nleds * sizeof(Monochrome);

    auto input = new ControlHubInput<Monochrome>(
        nleds,
        &hyp->hub,
        {
            {.column = COL_COLANDER1, .slot = 0, .pattern = new Colander::OnPattern()},
            {.column = COL_COLANDER1, .slot = 1, .pattern = new Colander::GlowPattern()},
            {.column = COL_COLANDER1, .slot = 2, .pattern = new Colander::SinPattern()},
            {.column = COL_COLANDER1, .slot = 3, .pattern = new Colander::BeatSingleFadePattern()},
            {.column = COL_COLANDER1, .slot = 4, .pattern = new Colander::BeatMultiFadePattern()},
            {.column = COL_COLANDER1, .slot = 5, .pattern = new Colander::BlinderPattern()},
            {.column = COL_COLANDER1, .slot = 6, .pattern = new Colander::SlowStrobePattern()},
            {.column = COL_COLANDER1, .slot = 7, .pattern = new Colander::GlitchPattern()},

            {.column = COL_COLANDER2, .slot = 0, .pattern = new Colander::FastStrobePattern()},
            {.column = COL_COLANDER2, .slot = 1, .pattern = new Colander::BeatAllFadePattern()},
            {.column = COL_COLANDER2, .slot = 2, .pattern = new Colander::BeatShakePattern()},
            {.column = COL_COLANDER2, .slot = 3, .pattern = new Colander::BeatStepPattern()},
            {.column = COL_COLANDER2, .slot = 4, .pattern = new Colander::GlowOriginalPattern()},
            {.column = COL_COLANDER2, .slot = 5, .pattern = new Colander::FastStrobePattern2()},
            {.column = COL_COLANDER2, .slot = 6, .pattern = new Colander::LFOPattern<PWM>()},
            {.column = COL_COLANDER2, .slot = 7, .pattern = new Colander::LFOPattern<SawDown>()},
        });

    auto splitInput = new InputSlicer(
        input,
        {
            {0, nbytes, true},
            {0, nbytes, false},
            {0, nbytes, false},
        });

    auto pipe = new ConvertPipe<Monochrome,Monochrome>(
        splitInput->getInput(0),
        new UDPOutput("hyperslave6.local", 9619, 60),
        incandescentLut8);
    hyp->addPipe(pipe);

    hyp->addPipe(
        new ConvertPipe<Monochrome, RGB>(
            splitInput->getInput(1),
            new MonitorOutput3d(&hyp->webServer, &colanderMap3dCombined, 60, 0.04)));

    // hyp->addPipe(
    //     new ConvertPipe<Monochrome, RGB>(
    //         splitInput->getInput(2),
    //         new MonitorOutput(&hyp->webServer, &colanderMap)));
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
