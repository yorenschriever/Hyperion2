// This project contains common code patterns that can be used to quickly 
// setup new projects.
// This file is an example where all these functions come together.

#include "core/hyperion.hpp"
#include "distributeAndMonitor.hpp"
#include "mapHelpers.hpp"
#include "patterns/patterns-led.hpp"
#include "patterns/patterns-monochrome.hpp"
#include "paletteColumn.hpp"

/* TODOs
 * - distributeAndMonitor3d
 * - mapping helpers for 3d
 * - rgb patterns
 * - rgb mapped patterns
 * - monochrome patterns
 * - monochrome mapped patterns
 * - default palette column creator
 * 
 * - add python turtle code here
 * - refactor old projects to use this turtle code
 * 
 * - add more examples
*/

void setupLed(Hyperion * hyp) {
    const int columnIndex = 1;

    // auto map = circleMap(100, 0.5);
    PixelMap *map = new PixelMap(combineMaps({
        circleMap(50, 0.5),
        circleMap(50, 0.7),
    }));

    // auto map = gridMap(5,5);
    // auto map = panelizeMap(circleMap(100, 1),2,2,1,1);

    auto input = new ControlHubInput<RGBA>(
        map->size(),
        &hyp->hub,
        columnIndex,
        {
            new LedPatterns::OnPattern({255,255,255},"White"),
            new LedPatterns::PalettePattern(0,"Primary"),
            new LedPatterns::PalettePattern(1,"Secondary"),
            new LedPatterns::GlowPulsePattern(),
            new LedPatterns::SegmentChasePattern(),
            new LedPatterns::FlashesPattern(),
            new LedPatterns::StrobePattern(),
            new LedPatterns::PixelGlitchPattern(),
            new LedPatterns::FadingNoisePattern(),
            new LedPatterns::StrobeHighlightPattern(),
            new LedPatterns::GradientChasePattern(),

        }
    );
    distributeAndMonitor<GRB,RGBA>(hyp, input, map, {
        {"hyperslave3.local", 9611, 25},
        {"hyperslave3.local", 9615, 25},
        {"hyperslave5.local", 9611, 25},
        {"hyperslave5.local", 9615, 25},
    });

    hyp->hub.setColumnName(columnIndex, "Led");
}

void setupMonochrome(Hyperion * hyp) {
    const int columnIndex = 2;

    auto map = new PixelMap(circleMap(30, 0.9));

    auto input = new ControlHubInput<Monochrome>(
        map->size(),
        &hyp->hub,
        columnIndex,
        {
            // new MonochromePatterns::SinPattern(),
            new MonochromePatterns::GlowPattern(),
            new MonochromePatterns::FastStrobePattern(),
            new MonochromePatterns::SlowStrobePattern(),
            new MonochromePatterns::FastStrobePattern2(),
            new MonochromePatterns::BlinderPattern(),
            new MonochromePatterns::BeatAllFadePattern(),
            new MonochromePatterns::BeatShakePattern(),
            new MonochromePatterns::BeatSingleFadePattern(),
            new MonochromePatterns::BeatMultiFadePattern(),
            new MonochromePatterns::GlitchPattern(),
            new MonochromePatterns::OnPattern(255),
            new MonochromePatterns::LFOPattern<NegativeCosFast>("Sin"),
            new MonochromePatterns::LFOPattern<SawDown>("SawDown"),
            new MonochromePatterns::BeatStepPattern(),
        }
    );
    distributeAndMonitor<Monochrome,Monochrome>(hyp, input, map, {
        {"hyperslave4.local", 9611, (int) map->size()},
    });

    hyp->hub.setColumnName(columnIndex, "Monochrome");
}

int main()
{
    auto hyp = new Hyperion();

    setupLed(hyp);
    setupMonochrome(hyp);
    setupPaletteColumn();

    // addPaletteColumn(hyp);
    // addTrianglesPipe(hyp);
    // // addLedparPipe(hyp);
    // // addBlinderPipe(hyp);
    // // addEyesPipe(hyp);
    // addOphanimPipe(hyp);

    // // Tempo::AddSource(new ConstantTempo(120));

    // // select first palette
    // hyp->hub.buttonPressed(0, 0);

    // hyp->hub.setColumnName(COL_PALETTE, "Palette");
    // hyp->hub.setColumnName(COL_VOL, "Vol");    
    // hyp->hub.setColumnName(COL_MONOCHROME, "Monochrome");
    // hyp->hub.setColumnName(COL_GRADIENT, "Gradient");
    // hyp->hub.setColumnName(COL_VIDEO, "Video");
    // hyp->hub.setColumnName(COL_VIDEO2, "Video 2");
    // hyp->hub.setColumnName(COL_MASK, "Mask");
    // hyp->hub.setColumnName(COL_FLASH, "Flash");

    // hyp->hub.setColumnName(COL_OPHANIM, "Oph");
    // hyp->hub.setColumnName(COL_OPHANIM_VIDEO, "Oph Video");
    // hyp->hub.setColumnName(COL_OPHANIM_MASK, "Oph Mask");
    // hyp->hub.setColumnName(COL_OPHANIM_FLASH, "Oph Flash");
    // hyp->hub.setColumnName(COL_OPHANIM_HALO, "Oph Halo");

    // // hyp->hub.setColumnName(COL_DEBUG, "Debug");
    // // hyp->hub.setColumnName(COL_DEBUG2, "Debug Oph");

    // hyp->hub.setFlashColumn(COL_FLASH);
    // hyp->hub.setFlashColumn(COL_OPHANIM_FLASH);
    // hyp->hub.setFlashColumn(0, false, true);
    // hyp->hub.setForcedSelection(0);

    // // hyp->setMidiControllerFactory(new MidiControllerFactoryFvf());

    hyp->start();
    // setViewParams(hyp, &viewParamsFront);
    // // setViewParams(hyp, viewParamsTop);
    while (1)
        Thread::sleep(1000);
}

// void addTrianglesPipe(Hyperion *hyp)
// {
//     int i = 0;
//     auto trianglesInput = new ControlHubInput<RGBA>(
//         triangleMap3d.size(),
//         &hyp->hub,
//         {
//             {.column = COL_VOL, .slot = 0, .pattern = new Max::ChevronsPattern(&triangleMap3d)},
//             {.column = COL_VOL, .slot = 1, .pattern = new Max::ChevronsConePattern(&cTriangleMap3d)},
//             {.column = COL_VOL, .slot = 2, .pattern = new Max::RadialFadePattern(&cTriangleMap3d)},
//             {.column = COL_VOL, .slot = 3, .pattern = new Max::RadialGlitterFadePattern(&cTriangleMap3d)},
//             {.column = COL_VOL, .slot = 4, .pattern = new Max::AngularFadePattern(&cTriangleMap3d)},
//             {.column = COL_VOL, .slot = 5, .pattern = new Low::VerticallyIsolated(&cTriangleMap3d)},
//             {.column = COL_VOL, .slot = 6, .pattern = new Low::StaticGradientPattern(&triangleMap3d)},
//             {.column = COL_VOL, .slot = 7, .pattern = new Min::LineLaunch(&triangleMap3d)},

//             {.column = COL_MONOCHROME, .slot = 0, .pattern = new Min::RibbenClivePattern<Glow>(10000, 1, 0.15)},
//             {.column = COL_MONOCHROME, .slot = 1, .pattern = new Min::RibbenFlashPattern()},
//             {.column = COL_MONOCHROME, .slot = 2, .pattern = new Min::GrowingCirclesPattern(&triangleMap3d)},
//             {.column = COL_MONOCHROME, .slot = 3, .pattern = new Min::SpiralPattern(&cTriangleMap3d)},
//             {.column = COL_MONOCHROME, .slot = 4, .pattern = new Low::HorizontalSaw(&cTriangleMap3d)},
//             {.column = COL_MONOCHROME, .slot = 5, .pattern = new Low::RadialSaw(&cTriangleMap3d)},
//             {.column = COL_MONOCHROME, .slot = 6, .pattern = new Mid::Lighthouse(&cTriangleMap3d)},
//             {.column = COL_MONOCHROME, .slot = 7, .pattern = new Hi::XY(&triangleMap3d)},
//             {.column = COL_MONOCHROME, .slot = 8, .pattern = new Hi::Z(&triangleMap3d)},

//             {.column = COL_GRADIENT, .slot = 0, .pattern = new Low::HorizontalSin(&cTriangleMap3d)},
//             {.column = COL_GRADIENT, .slot = 1, .pattern = new Low::GrowShrink(&cTriangleMap3d)},
//             {.column = COL_GRADIENT, .slot = 2, .pattern = new Low::RotatingRingsPattern(&cTriangleMap3d)},
//             {.column = COL_GRADIENT, .slot = 3, .pattern = new Low::GlowPulsePattern(&triangleMap3d)},
//             {.column = COL_GRADIENT, .slot = 4, .pattern = new Min::SegmentChasePattern()},
//             {.column = COL_GRADIENT, .slot = 5, .pattern = new Min::GlowPulsePattern()},
//             {.column = COL_GRADIENT, .slot = 6, .pattern = new Mid::TopChase(&cTriangleMap3d)},
//             {.column = COL_GRADIENT, .slot = 7, .pattern = new Mid::FireworksPattern(&triangleMap3d)},

//             {.column = COL_VIDEO, .slot = 0, .pattern = new VideoPalettePattern("videos/processed/DriehoekLos_Wipe_01.bin", "Wipe 01", 180)},
//             {.column = COL_VIDEO, .slot = 1, .pattern = new VideoPalettePattern("videos/processed/DriehoekLos_Wipe_02.bin", "Wipe 02", 180)},
//             {.column = COL_VIDEO, .slot = 2, .pattern = new VideoPalettePattern("videos/processed/DriehoekLos_Wipe_03.bin", "Wipe 03", 180)},
//             {.column = COL_VIDEO, .slot = 3, .pattern = new VideoPalettePattern("videos/processed/DriehoekLos_Wipe_04.bin", "Wipe 04", 180)},
//             {.column = COL_VIDEO, .slot = 4, .pattern = new VideoPalettePattern("videos/processed/DriehoekLos_Wipe_05.bin", "Wipe 05", 180)},
//             {.column = COL_VIDEO, .slot = 5, .pattern = new VideoPalettePattern("videos/processed/DriehoekenGroep_Fade_01.bin", "Fade 01", 180)},
//             {.column = COL_VIDEO, .slot = 6, .pattern = new VideoPalettePattern("videos/processed/DriehoekenGroep_Tracer_02.bin", "Tracer 02", 180)},
//             {.column = COL_VIDEO, .slot = 7, .pattern = new VideoPalettePattern("videos/processed/DriehoekenGroep_Wipe_02_[kan_90_-_180_graden_gedraaid].bin", "Wipe 02", 180)},
            
//             {.column = COL_VIDEO2, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/DriehoekLos_Cirkels_03.bin", "Cirkels 03", 180)},
//             {.column = COL_VIDEO2, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/DriehoekLos_Cirkels_05.bin", "Cirkels 05", 180)},
//             {.column = COL_VIDEO2, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/DriehoekLos_Grid_01.bin", "Los Grid 01", 180)},
//             {.column = COL_VIDEO2, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/DriehoekLos_Grid_02.bin", "Los Grid 02", 180)},
//             {.column = COL_VIDEO2, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/DriehoekLos_Heen_en_Weer_01.bin", "Heen en Weer 01", 180)},
//             {.column = COL_VIDEO2, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/DriehoekLos_Heen_en_Weer_02.bin", "Heen en Weer 02", 180)},
//             {.column = COL_VIDEO2, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/DriehoekLos_Triangle_01.bin", "Triangle 01", 180)},
//             {.column = COL_VIDEO2, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/DriehoekLos_Wave_04.bin", "Wave 04", 180)},
//             {.column = COL_VIDEO2, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/DriehoekenGroep_Noise_01_[kan_90_-_180_graden_gedraaid].bin", "Noise 01", 180)},
//             {.column = COL_VIDEO2, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/DriehoekenGroep_Noise_02_[kan_90_-_180_graden_gedraaid].bin", "Noise 02", 180)},
//             {.column = COL_VIDEO2, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/DriehoekenGroep_Tracer_01.bin", "Tracer 01", 180)},
//             {.column = COL_VIDEO2, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/DriehoekenGroep_Wipe_01_[kan_90_-_180_graden_gedraaid].bin", "Wipe 01", 180)},

//             {.column = COL_VIDEO2, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/DriehoekLos_Cirkels_01.bin", "Cirkels 01", 180)},
//             {.column = COL_VIDEO2, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/DriehoekLos_Cirkels_02.bin", "Cirkels 02", 180)},
//             {.column = COL_VIDEO2, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/DriehoekLos_Cirkels_04.bin", "Cirkels 04", 180)},
//             {.column = COL_VIDEO2, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/DriehoekLos_Grid_03.bin", "Los Grid 03", 180)},
//             {.column = COL_VIDEO2, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/DriehoekLos_Grid_04.bin", "Los Grid 04", 180)},
//             {.column = COL_VIDEO2, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/DriehoekLos_Noise_01.bin", "Noise 01", 180)},
//             {.column = COL_VIDEO2, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/DriehoekLos_Noise_02.bin", "Noise 02", 180)},
//             {.column = COL_VIDEO2, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/DriehoekLos_Noise_03.bin", "Noise 03", 180)},
//             {.column = COL_VIDEO2, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/DriehoekLos_Spiraal_01.bin", "Spiraal 01", 180)},
//             {.column = COL_VIDEO2, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/DriehoekLos_Spiraal_02.bin", "Spiraal 02", 180)},
//             {.column = COL_VIDEO2, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/DriehoekLos_Tracer_01.bin", "Tracer 01", 180)},
//             {.column = COL_VIDEO2, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/DriehoekLos_Triangle_02.bin", "Triangle 02", 180)},
//             {.column = COL_VIDEO2, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/DriehoekLos_Triangle_03.bin", "Triangle 03", 180)},
//             {.column = COL_VIDEO2, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/DriehoekLos_Triangle_04.bin", "Triangle 04", 180)},
//             {.column = COL_VIDEO2, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/DriehoekLos_Wave_01.bin", "Wave 01", 180)},
//             {.column = COL_VIDEO2, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/DriehoekLos_Wave_02.bin", "Wave 02", 180)},
//             {.column = COL_VIDEO2, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/DriehoekLos_Wave_03.bin", "Wave 03", 180)},


//             {.column = COL_MASK, .slot = 0, .pattern = new Mask::SinChaseMaskPattern()},
//             {.column = COL_MASK, .slot = 1, .pattern = new Mask::GlowPulseMaskPattern()},
//             {.column = COL_MASK, .slot = 2, .pattern = new Mask::XY(&triangleMap3d)},
//             {.column = COL_MASK, .slot = 3, .pattern = new Mask::HorizontalSaw(&cTriangleMap3d)},
//             {.column = COL_MASK, .slot = 4, .pattern = new Mask::RadialSaw(&cTriangleMap3d)},
//             {.column = COL_MASK, .slot = 5, .pattern = new VideoPattern("videos/processed/Mask_DriehoekenGroep_Wipe_01.bin", "Wipe 01")},
//             // {.column = COL_MASK, .slot = 5, .pattern = new Mask::FireworksPattern(&triangleMap3d)},
//             {.column = COL_MASK, .slot = 6, .pattern = new VideoPattern("videos/processed/Mask_Spiraal_01.bin", "Mask_Spiraal_01")},
//             {.column = COL_MASK, .slot = 7, .pattern = new VideoPattern("videos/processed/Mask_DriehoekenGroep_Fade_01.bin", "Fade_01")},
//             {.column = COL_MASK, .slot = 8, .pattern = new VideoPattern("videos/processed/Mask_DriehoekenGroep_Fade_02.bin", "Fade_02")},
//             {.column = COL_MASK, .slot = 9, .pattern = new VideoPattern("videos/processed/Mask_DriehoekenGroep_Tracer_01.bin", "Tracer_01")},
//             {.column = COL_MASK, .slot = 10, .pattern = new VideoPattern("videos/processed/Mask_Noise_01.bin", "Mask_Noise_01")},
            
//             {.column = COL_FLASH, .slot = 0, .pattern = new Flash::FlashesPattern()},
//             {.column = COL_FLASH, .slot = 1, .pattern = new Flash::SquareGlitchPattern(&triangleMap3d)},
//             {.column = COL_FLASH, .slot = 2, .pattern = new Flash::PixelGlitchPattern()},
//             {.column = COL_FLASH, .slot = 3, .pattern = new Max::GrowingStrobePattern(&cTriangleMap3d)},
//             {.column = COL_FLASH, .slot = 4, .pattern = new Flash::StrobeHighlightPattern()},
//             {.column = COL_FLASH, .slot = 5, .pattern = new Flash::StrobePattern()},
//             {.column = COL_FLASH, .slot = 6, .pattern = new Flash::FadingNoisePattern()},

//             // {.column = COL_DEBUG, .slot = 0, .pattern = new TestPatterns::ShowStarts(60)},
//             // {.column = COL_DEBUG, .slot = 1, .pattern = new TestPatterns::OneColor(RGB(255, 0, 0), "Red")},
//             // {.column = COL_DEBUG, .slot = 2, .pattern = new TestPatterns::OneColor(RGB(0, 255, 0), "Green")},
//             // {.column = COL_DEBUG, .slot = 3, .pattern = new TestPatterns::OneColor(RGB(0, 0, 255), "Blue")},
//             // {.column = COL_DEBUG, .slot = 4, .pattern = new TestPatterns::OneColor(RGB(255, 255, 255), "White")},
//             // {.column = COL_DEBUG, .slot = 5, .pattern = new TestPatterns::OneColor(RGB(127, 127, 127), "White 50%")},
//             // {.column = COL_DEBUG, .slot = 6, .pattern = new TestPatterns::Palette(120, 20)},
//             // {.column = COL_DEBUG, .slot = 7, .pattern = new TestPatterns::Gamma(60)},
//             // {.column = COL_DEBUG, .slot = 8, .pattern = new TestPatterns::BrightnessMatch()},

//         });

//     int triangleSize = 3*60 * sizeof(RGBA);
//     const int numSlices = 9;

//     auto splitInput = new InputSlicer(
//         trianglesInput,
//         {
//          {0  * triangleSize, 2 * triangleSize, true},//1.2
//          {2  * triangleSize, 2 * triangleSize, true},//1.3
//          {4  * triangleSize, 2 * triangleSize, true},//1.4

//          {6  * triangleSize, 2 * triangleSize, true},//2.1
//          {8  * triangleSize, 1 * triangleSize, true},//2.2
//          {9  * triangleSize, 1 * triangleSize, true},//2.3
//          {10 * triangleSize, 2 * triangleSize, true},//2.5
//          {12 * triangleSize, 2 * triangleSize, true},//2.6
//          {14 * triangleSize, 1 * triangleSize, true},//2.7

//          {0, 16 * triangleSize, false}});

//     typedef struct
//     {
//         const char *host;
//         const unsigned short port;
//     } Slave;
//     Slave slaves[numSlices] = {
//         {.host = "hyperslave3.local", .port = 9612}, 
//         {.host = "hyperslave3.local", .port = 9613}, 
//         {.host = "hyperslave3.local", .port = 9614}, 
         
//         {.host = "hyperslave2.local", .port = 9611}, 
//         {.host = "hyperslave2.local", .port = 9612},
//         {.host = "hyperslave2.local", .port = 9613}, 
//         {.host = "hyperslave2.local", .port = 9615}, 
//         {.host = "hyperslave2.local", .port = 9616}, 
//         {.host = "hyperslave2.local", .port = 9617}, 

//         // {.host = "hyperslave3.local", .port = 9611}, 
//         // {.host = "hyperslave3.local", .port = 9612}, 
//         // {.host = "hyperslave3.local", .port = 9613}, 
//         // {.host = "hyperslave3.local", .port = 9614}, 
//     };

//     for (int i = 0; i < numSlices; i++)
//     {
//         auto pipe = new ConvertPipe<RGBA, GBR>(
//             splitInput->getInput(i),
//             new UDPOutput(slaves[i].host, slaves[i].port, 60),
//             trianglesLut);
//         hyp->addPipe(pipe);
//     }

//     hyp->addPipe(
//         new ConvertPipe<RGBA, RGB>(
//             splitInput->getInput(numSlices),
//             new MonitorOutput3d(&hyp->webServer, &triangleMap3d)));

//     // hyp->addPipe(
//     //     new ConvertPipe<RGBA, RGB>(
//     //         trianglesInput,
//     //         new MonitorOutput(&hyp->webServer,&singleTriangleMap)));
// }

// // void addLedparPipe(Hyperion *hyp)
// // {
// //     auto ledparInput = new ControlHubInput<RGBA>(
// //         ledparMap3d.size(),
// //         &hyp->hub,
// //         {
// //             {.column = LEDPAR_COLUMN, .slot = 0, .pattern = new TestPatterns::ShowStarts(60)},
// //             {.column = LEDPAR_COLUMN, .slot = 1, .pattern = new TestPatterns::OneColor(RGB(255, 0, 0), "Red")},
// //             {.column = LEDPAR_COLUMN, .slot = 2, .pattern = new TestPatterns::OneColor(RGB(0, 255, 0), "Green")},
// //             {.column = LEDPAR_COLUMN, .slot = 3, .pattern = new TestPatterns::OneColor(RGB(0, 0, 255), "Blue")},
// //             {.column = LEDPAR_COLUMN, .slot = 4, .pattern = new TestPatterns::OneColor(RGB(255, 255, 255), "White")},
// //             {.column = LEDPAR_COLUMN, .slot = 5, .pattern = new TestPatterns::OneColor(RGB(127, 127, 127), "White 50%")},
// //             {.column = LEDPAR_COLUMN, .slot = 6, .pattern = new TestPatterns::Palette(120, 20)},
// //             {.column = LEDPAR_COLUMN, .slot = 7, .pattern = new TestPatterns::Gamma(60)},
// //             {.column = LEDPAR_COLUMN, .slot = 8, .pattern = new TestPatterns::BrightnessMatch()},
// //         });

// //     hyp->addPipe(
// //         new ConvertPipe<RGBA, RGB>(
// //             ledparInput,
// //             new MonitorOutput3d(&hyp->webServer, &ledparMap3d, 60, 0.05)));
// // }

// // void addBlinderPipe(Hyperion *hyp)
// // {
// //     auto blinderInput = new ControlHubInput<RGBA>(
// //         blinderMap3d.size(),
// //         &hyp->hub,
// //         {
// //             {.column = BLINDER_COLUMN, .slot = 0, .pattern = new TestPatterns::ShowStarts(60)},
// //             {.column = BLINDER_COLUMN, .slot = 1, .pattern = new TestPatterns::OneColor(RGB(255, 0, 0), "Red")},
// //             {.column = BLINDER_COLUMN, .slot = 2, .pattern = new TestPatterns::OneColor(RGB(0, 255, 0), "Green")},
// //             {.column = BLINDER_COLUMN, .slot = 3, .pattern = new TestPatterns::OneColor(RGB(0, 0, 255), "Blue")},
// //             {.column = BLINDER_COLUMN, .slot = 4, .pattern = new TestPatterns::OneColor(RGB(255, 255, 255), "White")},
// //             {.column = BLINDER_COLUMN, .slot = 5, .pattern = new TestPatterns::OneColor(RGB(127, 127, 127), "White 50%")},
// //             {.column = BLINDER_COLUMN, .slot = 6, .pattern = new TestPatterns::Palette(120, 20)},
// //             {.column = BLINDER_COLUMN, .slot = 7, .pattern = new TestPatterns::Gamma(60)},
// //             {.column = BLINDER_COLUMN, .slot = 8, .pattern = new TestPatterns::BrightnessMatch()},
// //         });

// //     hyp->addPipe(
// //         new ConvertPipe<RGBA, RGB>(
// //             blinderInput,
// //             new MonitorOutput3d(&hyp->webServer, &blinderMap3d, 60, 0.05)));
// // }

// // void addEyesPipe(Hyperion *hyp)
// // {
// //     auto eyesInput = new ControlHubInput<RGBA>(
// //         eyesMap3d.size(),
// //         &hyp->hub,
// //         {
// //             {.column = EYES_COLUMN, .slot = 0, .pattern = new TestPatterns::ShowStarts(60)},
// //             {.column = EYES_COLUMN, .slot = 1, .pattern = new TestPatterns::OneColor(RGB(255, 0, 0), "Red")},
// //             {.column = EYES_COLUMN, .slot = 2, .pattern = new TestPatterns::OneColor(RGB(0, 255, 0), "Green")},
// //             {.column = EYES_COLUMN, .slot = 3, .pattern = new TestPatterns::OneColor(RGB(0, 0, 255), "Blue")},
// //             {.column = EYES_COLUMN, .slot = 4, .pattern = new TestPatterns::OneColor(RGB(255, 255, 255), "White")},
// //             {.column = EYES_COLUMN, .slot = 5, .pattern = new TestPatterns::OneColor(RGB(127, 127, 127), "White 50%")},
// //             {.column = EYES_COLUMN, .slot = 6, .pattern = new TestPatterns::Palette(120, 20)},
// //             {.column = EYES_COLUMN, .slot = 7, .pattern = new TestPatterns::Gamma(60)},
// //             {.column = EYES_COLUMN, .slot = 8, .pattern = new TestPatterns::BrightnessMatch()},
// //         });

// //     hyp->addPipe(
// //         new ConvertPipe<RGBA, RGB>(
// //             eyesInput,
// //             new MonitorOutput3d(&hyp->webServer, &eyesMap3d, 60, 0.05)));
// // }

// const char * ophanimHost = "hyperslave6.local";

// void addOphanimPipe(Hyperion *hyp)
// {
//     int i=0;

//     const int ring1Size = 2*152;
//     auto ring1Input = new ControlHubInput<RGBA>(
//         ring1Map.size(),
//         &hyp->hub,
//         {
//             {.column = COL_OPHANIM, .slot = 0, .pattern = new Ophanim::MonochromePattern()},
//             {.column = COL_OPHANIM, .slot = 1, .pattern = new Ophanim::StereochromePattern(0)},
//             {.column = COL_OPHANIM, .slot = 2, .pattern = new Ophanim::SinPattern()},
//             {.column = COL_OPHANIM, .slot = 3, .pattern = new Ophanim::ChasePattern()},
//             {.column = COL_OPHANIM, .slot = 4, .pattern = new Ophanim::SinStripPattern()},
//             {.column = COL_OPHANIM, .slot = 5, .pattern = new Ophanim::SinStripPattern2()},
//             {.column = COL_OPHANIM, .slot = 6, .pattern = new Ophanim::OnbeatFadeAllPattern()},
//             {.column = COL_OPHANIM, .slot = 7, .pattern = new Ophanim::SlowStrobePattern()},

//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring1/Cirkels_Cirkels_01.bin","Cirkels 01")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring1/Cirkels_Cirkels_02.bin","Cirkels 02")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring1/Cirkels_Cirkels_03.bin","Cirkels 03")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring1/Cirkels_Cirkels_04.bin","Cirkels 04")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring1/Cirkels_Cirkels_05.bin","Cirkels 05")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring1/Cirkels_Cirkels_06.bin","Cirkels 06")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring1/Cirkels_Cirkels_07.bin","Cirkels 07")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring1/Cirkels_Grid_01.bin","Grid 01")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring1/Cirkels_Noise_01.bin","Noise 01")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring1/Cirkels_Noise_02.bin","Noise 02")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring1/Cirkels_Spiraal_01.bin","Spiraal 01")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring1/Cirkels_Spiraal_02.bin","Spiraal 02")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring1/Cirkels_Spiraal_03.bin","Spiraal 03")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring1/Cirkels_Spiraal_04.bin","Spiraal 04")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring1/Cirkels_Tracers_01.bin","Tracers 01")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring1/Cirkels_Tracers_02.bin","Tracers 02")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring1/Cirkels_Tracers_03.bin","Tracers 03")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring1/Cirkels_Tracers_04.bin","Tracers 04")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring1/Cirkels_X_01.bin","X 01")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring1/Cirkels_X_02.bin","X 02")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring1/Cirkels_X_03.bin","X 03")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring1/Cirkels_X_04.bin","X 04")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring1/Cirkels_X_05.bin","X 05")},


//             {.column = COL_OPHANIM_MASK, .slot = 0, .pattern = new Ophanim::SinChaseMaskPattern()},
//             {.column = COL_OPHANIM_MASK, .slot = 1, .pattern = new Ophanim::GlowPulseMaskPattern()},

//             {.column = COL_OPHANIM_FLASH, .slot = 0, .pattern = new Ophanim::MonochromePattern()},
//             {.column = COL_OPHANIM_FLASH, .slot = 1, .pattern = new Ophanim::SquareGlitchPattern()},
//             {.column = COL_OPHANIM_FLASH, .slot = 2, .pattern = new Ophanim::OnbeatFadeAllPattern()},

//             // {.column = COL_DEBUG, .slot = 0, .pattern = new TestPatterns::ShowStarts(ring1Size/2)},
//             // {.column = COL_DEBUG, .slot = 1, .pattern = new TestPatterns::OneColor(RGB(255, 0, 0), "Red")},
//             // {.column = COL_DEBUG, .slot = 2, .pattern = new TestPatterns::OneColor(RGB(0, 255, 0), "Green")},
//             // {.column = COL_DEBUG, .slot = 3, .pattern = new TestPatterns::OneColor(RGB(0, 0, 255), "Blue")},
//             // {.column = COL_DEBUG, .slot = 4, .pattern = new TestPatterns::OneColor(RGB(255, 255, 255), "White")},
//             // {.column = COL_DEBUG, .slot = 5, .pattern = new TestPatterns::OneColor(RGB(127, 127, 127), "White 50%")},
//             // {.column = COL_DEBUG, .slot = 6, .pattern = new TestPatterns::Palette(120, 20)},
//             // {.column = COL_DEBUG, .slot = 7, .pattern = new TestPatterns::Gamma(60)},
//             // {.column = COL_DEBUG, .slot = 8, .pattern = new TestPatterns::BrightnessMatch()},

//             // {.column = COL_OPHANIM, .slot = 6, .pattern = new Ophanim::GradientPattern()},
//             // {.column = COL_OPHANIM, .slot = 7, .pattern = new Ophanim::HoepelsTransition()},

            
//             // {.column = COL_OPHANIM, .slot = 9, .pattern = new Ophanim::OnbeatFadePattern()},
//             // {.column = COL_OPHANIM, .slot = 10, .pattern = new Ophanim::FireworkPattern()},
            
//             // {.column = COL_OPHANIM, .slot = 11, .pattern = new Ophanim::AntichasePattern()},
           
//             // {.column = COL_OPHANIM, .slot = 12, .pattern = new Ophanim::ClivePattern<SinFast>(0, 500, 2000)},
//             // {.column = COL_OPHANIM, .slot = 13, .pattern = new Ophanim::ClivePattern<SinFast>(1, 10, 2000)},
//             // {.column = COL_OPHANIM, .slot = 14, .pattern = new Ophanim::GlowPulsePattern()},

//             // 
//             // {.column = COL_OPHANIM, .slot = 16, .pattern = new Ophanim::SquareGlitchPattern()},
//             // {.column = COL_OPHANIM, .slot = 17, .pattern = new Ophanim::ClivePattern<SawDown>(1, 25, 500, 1, 0.1)},
//         });

//     // hyp->addPipe(
//     //     new ConvertPipe<RGBA, RGB>(
//     //         ring1Input,
//     //         new MonitorOutput(&hyp->webServer, &ring1Map, 60, 0.025)));

//     // hyp->addPipe(new ConvertPipe<RGBA, BGR>(
//     //         ring1Input,
//     //         new UDPOutput(ophanimHost, 9611, 60),
//     //         trianglesLut));

//     auto ring1SplitInput = new InputSlicer(
//         ring1Input,
//         {{0, ring1Size/2*sizeof(RGBA), true},
//          {ring1Size/2*sizeof(RGBA), ring1Size/2*sizeof(RGBA), true},
//          {0, ring1Size*sizeof(RGBA), false}});

//     hyp->addPipe(new ConvertPipe<RGBA, GBR>(
//             ring1SplitInput->getInput(0),
//             new UDPOutput(ophanimHost, 9611, 60),
//             trianglesLut));

//     hyp->addPipe(new ConvertPipe<RGBA, GBR>(
//             ring1SplitInput->getInput(1),
//             new UDPOutput(ophanimHost, 9612, 60),
//             trianglesLut));

//     hyp->addPipe(
//         new ConvertPipe<RGBA, RGB>(
//             ring1SplitInput->getInput(2),
//             new MonitorOutput(&hyp->webServer, &ring1Map, 60, 0.025)));

//     auto ring2Input = new ControlHubInput<Monochrome>(
//         ring2Map.size(),
//         &hyp->hub,
//         {
//             {.column = COL_OPHANIM_HALO, .slot = 0, .pattern = new MonochromePatterns::BlinderPattern()},
//             {.column = COL_OPHANIM_HALO, .slot = 1, .pattern = new MonochromePatterns::SinPattern()},
//             {.column = COL_OPHANIM_HALO, .slot = 2, .pattern = new MonochromePatterns::GlowPattern()},
//             {.column = COL_OPHANIM_HALO, .slot = 3, .pattern = new MonochromePatterns::LFOPattern<SoftSawDown>("Soft Saw Down")},
//             {.column = COL_OPHANIM_HALO, .slot = 4, .pattern = new MonochromePatterns::BeatShakePattern()},
//             {.column = COL_OPHANIM_HALO, .slot = 5, .pattern = new MonochromePatterns::BeatMultiFadePattern()},
//             {.column = COL_OPHANIM_HALO, .slot = 6, .pattern = new MonochromePatterns::LFOPattern<Glow>("Neg Cos")},
//             {.column = COL_OPHANIM_HALO, .slot = 7, .pattern = new MonochromePatterns::BeatSingleFadePattern()},

//             {.column = COL_OPHANIM_HALO, .slot = 8, .pattern = new MonochromePatterns::LFOPattern<Sin>("Sin")},
//             {.column = COL_OPHANIM_HALO, .slot = 9, .pattern = new MonochromePatterns::LFOPattern<PWM>("PWM")},
//             {.column = COL_OPHANIM_HALO, .slot = 10, .pattern = new MonochromePatterns::BeatStepPattern()},
//             {.column = COL_OPHANIM_HALO, .slot = 11, .pattern = new MonochromePatterns::SlowStrobePattern()},
//             {.column = COL_OPHANIM_HALO, .slot = 12, .pattern = new MonochromePatterns::BeatAllFadePattern()},
//             {.column = COL_OPHANIM_HALO, .slot = 13, .pattern = new MonochromePatterns::OnPattern()},

//             {.column = COL_OPHANIM_FLASH, .slot = 4, .pattern = new MonochromePatterns::OnPattern()},
//             {.column = COL_OPHANIM_FLASH, .slot = 5, .pattern = new MonochromePatterns::GlitchPattern()},
//             {.column = COL_OPHANIM_FLASH, .slot = 6, .pattern = new MonochromePatterns::BeatAllFadePattern()},

            
//         });

//     // hyp->addPipe(
//     //     new ConvertPipe<Monochrome, RGB>(
//     //         ring2Input,
//     //         new MonitorOutput(&hyp->webServer, &ring2Map, 60, 0.025)));

//         // hyp->addPipe(new Pipe(
//         //     ring2Input,
//         //     new UDPOutput(ophanimHost, 9621, 60)));

//     const int ring2Size = 24*sizeof(Monochrome);
//     auto ring2SplitInput = new InputSlicer(
//         ring2Input,
//         {{0, ring2Size, true},
//          {0, ring2Size, false}});

//     hyp->addPipe(new ConvertPipe<Monochrome, Monochrome>(
//             ring2SplitInput->getInput(0),
//             new UDPOutput(ophanimHost, 9621, 60),
//             ophanimLut
//             ));

//     hyp->addPipe(
//         new ConvertPipe<Monochrome, RGB>(
//             ring2SplitInput->getInput(1),
//             new MonitorOutput(&hyp->webServer, &ring2Map, 60, 0.025)));

//     i=0;
//     const int ring3Size = 2*127;
//     auto ring3Input = new ControlHubInput<RGBA>(
//         ring3Map.size(),
//         &hyp->hub,
//         {
//             {.column = COL_OPHANIM, .slot = 0, .pattern = new Ophanim::MonochromePattern()},
//             {.column = COL_OPHANIM, .slot = 1, .pattern = new Ophanim::StereochromePattern(1)},
//             {.column = COL_OPHANIM, .slot = 2, .pattern = new Ophanim::SinPattern()},
//             {.column = COL_OPHANIM, .slot = 3, .pattern = new Ophanim::ChasePattern()},
//             {.column = COL_OPHANIM, .slot = 4, .pattern = new Ophanim::SinStripPattern()},
//             {.column = COL_OPHANIM, .slot = 5, .pattern = new Ophanim::SinStripPattern2()},
//             {.column = COL_OPHANIM, .slot = 6, .pattern = new Ophanim::OnbeatFadeAllPattern()},     
//             {.column = COL_OPHANIM, .slot = 7, .pattern = new Ophanim::SlowStrobePattern()},

//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring3/Cirkels_Cirkels_01.bin","Cirkels 01")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring3/Cirkels_Cirkels_02.bin","Cirkels 02")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring3/Cirkels_Cirkels_03.bin","Cirkels 03")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring3/Cirkels_Cirkels_04.bin","Cirkels 04")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring3/Cirkels_Cirkels_05.bin","Cirkels 05")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring3/Cirkels_Cirkels_06.bin","Cirkels 06")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring3/Cirkels_Cirkels_07.bin","Cirkels 07")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring3/Cirkels_Grid_01.bin","Grid 01")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring3/Cirkels_Noise_01.bin","Noise 01")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring3/Cirkels_Noise_02.bin","Noise 02")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring3/Cirkels_Spiraal_01.bin","Spiraal 01")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring3/Cirkels_Spiraal_02.bin","Spiraal 02")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring3/Cirkels_Spiraal_03.bin","Spiraal 03")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring3/Cirkels_Spiraal_04.bin","Spiraal 04")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring3/Cirkels_Tracers_01.bin","Tracers 01")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring3/Cirkels_Tracers_02.bin","Tracers 02")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring3/Cirkels_Tracers_03.bin","Tracers 03")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring3/Cirkels_Tracers_04.bin","Tracers 04")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring3/Cirkels_X_01.bin","X 01")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring3/Cirkels_X_02.bin","X 02")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring3/Cirkels_X_03.bin","X 03")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring3/Cirkels_X_04.bin","X 04")},
//             {.column = COL_OPHANIM_VIDEO, .slot = i++, .pattern = new VideoPalettePattern("videos/processed/ring3/Cirkels_X_05.bin","X 05")},

//             {.column = COL_OPHANIM_MASK, .slot = 0, .pattern = new Ophanim::SinChaseMaskPattern()},
//             {.column = COL_OPHANIM_MASK, .slot = 1, .pattern = new Ophanim::GlowPulseMaskPattern()},

//             {.column = COL_OPHANIM_FLASH, .slot = 0, .pattern = new Ophanim::MonochromePattern()},
//             {.column = COL_OPHANIM_FLASH, .slot = 1, .pattern = new Ophanim::SquareGlitchPattern()},
//             {.column = COL_OPHANIM_FLASH, .slot = 2, .pattern = new Ophanim::OnbeatFadeAllPattern()},

//             // {.column = COL_DEBUG, .slot = 0, .pattern = new TestPatterns::ShowStarts(ring3Size/2)},
//             // {.column = COL_DEBUG, .slot = 1, .pattern = new TestPatterns::OneColor(RGB(255, 0, 0), "Red")},
//             // {.column = COL_DEBUG, .slot = 2, .pattern = new TestPatterns::OneColor(RGB(0, 255, 0), "Green")},
//             // {.column = COL_DEBUG, .slot = 3, .pattern = new TestPatterns::OneColor(RGB(0, 0, 255), "Blue")},
//             // {.column = COL_DEBUG, .slot = 4, .pattern = new TestPatterns::OneColor(RGB(255, 255, 255), "White")},
//             // {.column = COL_DEBUG, .slot = 5, .pattern = new TestPatterns::OneColor(RGB(127, 127, 127), "White 50%")},
//             // {.column = COL_DEBUG, .slot = 6, .pattern = new TestPatterns::Palette(120, 20)},
//             // {.column = COL_DEBUG, .slot = 7, .pattern = new TestPatterns::Gamma(60)},
//             // {.column = COL_DEBUG, .slot = 8, .pattern = new TestPatterns::BrightnessMatch()},
//         });

//     // hyp->addPipe(
//     //     new ConvertPipe<RGBA, RGB>(
//     //         ring3Input,
//     //         new MonitorOutput(&hyp->webServer, &ring3Map, 60, 0.025)));

//     // hyp->addPipe(new ConvertPipe<RGBA, BGR>(
//     //         ring3Input,
//     //         new UDPOutput(ophanimHost, 9615, 60),
//     //         trianglesLut));

    
//     auto ring3SplitInput = new InputSlicer(
//         ring3Input,
//         {{0, ring3Size/2*sizeof(RGBA), true},
//          {ring3Size/2*sizeof(RGBA), ring3Size/2*sizeof(RGBA), true},
//          {0, ring3Size*sizeof(RGBA), false}});

//     hyp->addPipe(new ConvertPipe<RGBA, GBR>(
//             ring3SplitInput->getInput(0),
//             new UDPOutput(ophanimHost, 9613, 60),
//             trianglesLut));

//     hyp->addPipe(new ConvertPipe<RGBA, GBR>(
//             ring3SplitInput->getInput(1),
//             new UDPOutput(ophanimHost, 9614, 60),
//             trianglesLut));

//     hyp->addPipe(
//         new ConvertPipe<RGBA, RGB>(
//             ring3SplitInput->getInput(2),
//             new MonitorOutput(&hyp->webServer, &ring3Map, 60, 0.025)));

// }

// 