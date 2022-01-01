#include "core/hyperion.hpp"
#include "../common/distributeAndMonitor.hpp"
#include "../common/mapHelpers.hpp"
#include "../common/patterns/patterns-led.hpp"
#include "../common/patterns/patterns-monochrome.hpp"
#include "../common/paletteColumn.hpp"
#include "../common/videoPattern.hpp"

LUT *NeopixelLut = new ColourCorrectionLUT(1.5, 255, 255, 255, 255);

void setupLedSide(Hyperion * hyp) {
    const int columnIndex = 1;

    PixelMap *map = new PixelMap(combineMaps({
        gridMap(1, 8*60, 1.8/(8*60), 0.8, 0),
        gridMap(1, 8*60, 1.8/(8*60), -0.8, 0),
    }));

    auto input = new ControlHubInput<RGBA>(
        map->size(),
        &hyp->hub,
        columnIndex,
        {
            // new VideoPattern("videos/processed/Fire_Level_1.bin","Fire lvl 1"),
            // new VideoPattern("videos/processed/Fire_Level_2.bin","Fire lvl 2"),
            new VideoPattern("videos/processed/Fire_Level_3.bin","Fire lvl 3"),
            new VideoPattern("videos/processed/Fire_Level_4.bin","Fire lvl 4"),
            // new LedPatterns::OnPattern({255,255,255},"White"),
            // new LedPatterns::PalettePattern(0,"Primary"),
            // new LedPatterns::PalettePattern(1,"Secondary"),
            new LedPatterns::GlowPulsePattern(),
            new LedPatterns::SegmentChasePattern(),
            new LedPatterns::GradientChasePattern(),

            //new LedPatterns::FlashesPattern(),
            //new LedPatterns::StrobePattern(),
            new LedPatterns::PixelGlitchPattern(),
            new LedPatterns::FadingNoisePattern(),
            new LedPatterns::StrobeHighlightPattern(),
        }
    );

        // auto pipe = new ConvertPipe<RGBA, RGB>(
        //     input,
        //     new UDPOutput("hypernode1.local", 9611, 60));
        // hyp->addPipe(pipe);

    distributeAndMonitor<GBR,RGBA>(hyp, input, map, {
        {"hypernode1.local", 9611, 8*60},
        {"hypernode1.local", 9612, 8*60},
    }, NeopixelLut);

    hyp->hub.setFlashColumn(columnIndex,false,true);

    hyp->hub.setColumnName(columnIndex, "Side");
}

void setupLedBackdrop(Hyperion * hyp) {
    const int columnIndex = 2;

    PixelMap *map = new PixelMap(
        rotateMap(gridMap(60, 8, 2*1.8/(8*60), -0.6, 0),90)
    );

    auto input = new ControlHubInput<RGBA>(
        map->size(),
        &hyp->hub,
        columnIndex,
        {
            // new VideoPattern("videos/processed/Fire_Level_1.bin","Fire lvl 1"),
            // new VideoPattern("videos/processed/Fire_Level_2.bin","Fire lvl 2"),
            new VideoPattern("videos/processed/Fire_Level_3.bin","Fire lvl 3"),
            new VideoPattern("videos/processed/Fire_Level_4.bin","Fire lvl 4"),
            // new LedPatterns::OnPattern({255,255,255},"White"),
            // new LedPatterns::PalettePattern(0,"Primary"),
            // new LedPatterns::PalettePattern(1,"Secondary"),
            new LedPatterns::GlowPulsePattern(),
            new LedPatterns::GradientChasePattern(),
            new LedPatterns::SegmentChasePattern(),
            // new LedPatterns::FlashesPattern(),
            // new LedPatterns::StrobePattern(),
            new LedPatterns::PixelGlitchPattern(),
            new LedPatterns::FadingNoisePattern(),
            new LedPatterns::StrobeHighlightPattern(),
            

        }
    );

    distributeAndMonitor<GBR,RGBA>(hyp, input, map, {
        {"hypernode1.local", 9613, 8 * 60},
    }, NeopixelLut);

    hyp->hub.setFlashColumn(columnIndex,false,true);
    hyp->hub.setColumnName(columnIndex, "Backdrop");
}

void setupBulbs(Hyperion * hyp) {
    const int columnIndex = 3;

    auto map = new PixelMap(circleMap(9, 0.3));

    auto input = new ControlHubInput<Monochrome>(
        map->size(),
        &hyp->hub,
        columnIndex,
        {
            // new MonochromePatterns::BlinderPattern(),
            new MonochromePatterns::OnPattern(100),
            new MonochromePatterns::LFOPattern<NegativeCosFast>("Sin"),
            new MonochromePatterns::BeatMultiFadePattern(),
            new MonochromePatterns::BeatShakePattern(),
            new MonochromePatterns::BeatStepPattern(),

            new MonochromePatterns::SlowStrobePattern(),
            new MonochromePatterns::FastStrobePattern2(),
            new MonochromePatterns::BlinderPattern(),
           
            new MonochromePatterns::SinPattern(),
            new MonochromePatterns::GlowPattern(),
            new MonochromePatterns::FastStrobePattern(),
            new MonochromePatterns::BeatAllFadePattern(),
            new MonochromePatterns::BeatSingleFadePattern(),
            new MonochromePatterns::GlitchPattern(),
            new MonochromePatterns::OnPattern(255),
            new MonochromePatterns::LFOPattern<SawDown>("SawDown"),
        }
    );

    distributeAndMonitor<Monochrome12,Monochrome>(hyp, input, map, {
        {"hyperslave4.local", 9620 , (int) map->size()},
    });

    hyp->hub.setFlashColumn(columnIndex,false,true);
    hyp->hub.setColumnName(columnIndex, "Bulbs");
}

void setupDMX(Hyperion * hyp) {
    const int columnIndex = 4;

    auto map = new PixelMap(resizeAndTranslateMap(
        circleMap(4, 0.1),
        1,
        0, 0.5));

    auto input = new ControlHubInput<Monochrome>(
        map->size(),
        &hyp->hub,
        columnIndex,
        {
            new MonochromePatterns::OnPattern(100),
            // new MonochromePatterns::SinPattern(),
            new MonochromePatterns::LFOPattern<NegativeCosFast>("Sin"),
            new MonochromePatterns::LFOPattern<SawDown>("SawDown"),
            new MonochromePatterns::BeatStepPattern(),
            new MonochromePatterns::BeatMultiFadePattern(),

            new MonochromePatterns::SlowStrobePattern(),
            new MonochromePatterns::BlinderPattern(),
            new MonochromePatterns::GlitchPattern(),


            //

            new MonochromePatterns::GlowPattern(),
            new MonochromePatterns::FastStrobePattern(),
            
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
            
        }
    );
    distributeAndMonitor<Monochrome,Monochrome>(hyp, input, map, {
        {"hypernode1.local", 9621 , (int) map->size()},
    });

    hyp->hub.setFlashColumn(columnIndex,false,true);
    hyp->hub.setColumnName(columnIndex, "DMX");
}

int main()
{
    auto hyp = new Hyperion();

    setupLedSide(hyp);
    setupLedBackdrop(hyp);
    setupBulbs(hyp);
    setupDMX(hyp);
    setupPaletteColumn(hyp);

    hyp->hub.setFlashRow(5);
    hyp->hub.setFlashRow(6);
    hyp->hub.setFlashRow(7);

    hyp->start();

    while (1)
        Thread::sleep(1000);
}
