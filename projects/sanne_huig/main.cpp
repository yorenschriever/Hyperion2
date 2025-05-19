#include "core/hyperion.hpp"
#include "../common/distributeAndMonitor.hpp"
#include "../common/mapHelpers.hpp"
#include "../common/patterns/patterns-led.hpp"
#include "../common/patterns/patterns-monochrome.hpp"
#include "../common/paletteColumn.hpp"
#include "../common/videoPattern.hpp"

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
            new VideoPattern("videos/processed/Fire_Level_1.bin","Fire lvl 1"),
            new VideoPattern("videos/processed/Fire_Level_2.bin","Fire lvl 2"),
            new VideoPattern("videos/processed/Fire_Level_3.bin","Fire lvl 3"),
            new VideoPattern("videos/processed/Fire_Level_4.bin","Fire lvl 4"),
            // new LedPatterns::OnPattern({255,255,255},"White"),
            // new LedPatterns::PalettePattern(0,"Primary"),
            // new LedPatterns::PalettePattern(1,"Secondary"),
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
        {"hyperslave3.local", 9611, 8*60},
        {"hyperslave3.local", 9615, 8*60},
    });

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
            new VideoPattern("videos/processed/Fire_Level_1.bin","Fire lvl 1"),
            new VideoPattern("videos/processed/Fire_Level_2.bin","Fire lvl 2"),
            new VideoPattern("videos/processed/Fire_Level_3.bin","Fire lvl 3"),
            new VideoPattern("videos/processed/Fire_Level_4.bin","Fire lvl 4"),
            // new LedPatterns::OnPattern({255,255,255},"White"),
            // new LedPatterns::PalettePattern(0,"Primary"),
            // new LedPatterns::PalettePattern(1,"Secondary"),
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
        {"hyperslave3.local", 9612, 6 * 60},
    });

    hyp->hub.setColumnName(columnIndex, "Backdrop");
}

void setupBulbs(Hyperion * hyp) {
    const int columnIndex = 3;

    auto map = new PixelMap(circleMap(10, 0.3));

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
        {"hyperslave3.local", 9613, (int) map->size()},
    });

    hyp->hub.setColumnName(columnIndex, "Bulbs");
}

int main()
{
    auto hyp = new Hyperion();

    setupLedSide(hyp);
    setupLedBackdrop(hyp);
    setupBulbs(hyp);
    setupPaletteColumn(hyp);

    hyp->start();

    while (1)
        Thread::sleep(1000);
}
