#include "core/hyperion.hpp"
#include "mapping/haloMap.hpp"
#include "palettes.hpp"
#include "patterns-halo.hpp"

void addHaloPipe(Hyperion *hyp);
void addPaletteColumn(Hyperion *hyp);

int main()
{
    auto hyp = new Hyperion();

    addHaloPipe(hyp);
    addPaletteColumn(hyp);

    Tempo::AddSource(new ConstantTempo(120));

    // auto select first palette
    hyp->hub.buttonPressed(0, 0);

    hyp->hub.setColumnName(0, "Palette");
    hyp->hub.setColumnName(1, "Halo patterns");

    hyp->hub.setFlashColumn(0, false, true);
    hyp->hub.setForcedSelection(0);

    hyp->start();
    while (1)
        Thread::sleep(1000);
}

void addHaloPipe(Hyperion *hyp)
{
    auto input = new ControlHubInput<RGBA>(
        haloMap.size(),
        &hyp->hub,
        {
            {.column = 1, .slot = 0, .pattern = new Halo::PrimaryColorPattern()},
            {.column = 1, .slot = 1, .pattern = new Halo::SecondaryColorPattern()},
            {.column = 1, .slot = 2, .pattern = new Halo::SinAllPattern()},
            {.column = 1, .slot = 3, .pattern = new Halo::SinChasePattern()},
            {.column = 1, .slot = 4, .pattern = new Halo::SawChasePattern()},
            {.column = 1, .slot = 5, .pattern = new Halo::StrobePattern()},
        });

    hyp->addPipe(new ConvertPipe<RGBA, RGB>(
        input,
        new MonitorOutput(haloMap)));

}

void addPaletteColumn(Hyperion *hyp)
{
    auto paletteColumn = new PaletteColumn(
        &hyp->hub,
        0,
        {
            heatmap,
            sunset1,
        });
    hyp->hub.subscribe(paletteColumn);
}
