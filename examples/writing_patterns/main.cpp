#include "core/hyperion.hpp"
#include "mapping/haloMap.hpp"
#include "palettes.hpp"
#include "patterns.hpp"

// forward declartions. This lets us call the functions before we defined them.
void addHaloPipe(Hyperion *hyp);
void addPaletteColumn(Hyperion *hyp);

int main()
{
    // create an hyperion object
    auto hyp = new Hyperion();

    // add a pipe
    addHaloPipe(hyp);

    // add a column to the controller that lets you select a palette
    addPaletteColumn(hyp);

    // Crate a tempo source that give us a constant beat of 120 BPM.
    // Later you will see how patterns can access this information.
    Tempo::AddSource(new ConstantTempo(120));

    // auto select first palette
    hyp->hub.buttonPressed(0, 0);

    // These names are displayed in the monitor
    hyp->hub.setColumnName(0, "Palette");
    hyp->hub.setColumnName(1, "Basics");
    hyp->hub.setColumnName(2, "Helpers");
    hyp->hub.setColumnName(3, "Params");
    hyp->hub.setColumnName(4, "Mapping");

    // set some column properties, like flash mode, group release, forces selection.
    hyp->hub.setFlashColumn(0, false, true);
    hyp->hub.setForcedSelection(0);

    // This starts the main hyperion loop
    hyp->start();

    // After that there is nothing we have to do anymore.
    // This loops keep your application alive.
    // If your project needs additional login to con continuously, you can add it here
    // Most projects dont need this.
    while (1)
        Thread::sleep(1000);
}

void addHaloPipe(Hyperion *hyp)
{
    // In Hyperion, all pixel data comes from an input and in transferred to an
    // output by a pipe. 
    // The input is a ControlHubInput. This input reads control data coming from
    // the web interface or midi controller, and renders the patters.
    // The output is a MonitorOutput. This will display the generated patterns in your browser.
    // It is best to write all your pattern in the RGBA color space (red, green blue, alpha).
    // RGBA gives you color blending and pattern layering options. And having all the patterns
    // in your pattern library in the same color space makes it easy to re-use them.
    // The monitor needs data in RGB format, so we use a ConvertPipe to do the conversion.

    auto input = new ControlHubInput<RGBA>(
        haloMap.size(), //The number of leds it is going to ask the patterns to generate color data for. 
        &hyp->hub, //The control hub to connect to
        {
            {.column = 1, .slot = 0, .pattern = new ExamplePatterns::HelloWorld()},
            {.column = 1, .slot = 1, .pattern = new ExamplePatterns::Palette()},
            {.column = 1, .slot = 2, .pattern = new ExamplePatterns::PaletteGradient()},
            {.column = 1, .slot = 3, .pattern = new ExamplePatterns::Blending()},

            {.column = 2, .slot = 0, .pattern = new ExamplePatterns::LFOPattern()},
            {.column = 2, .slot = 1, .pattern = new ExamplePatterns::LFOChase()},
            {.column = 2, .slot = 2, .pattern = new ExamplePatterns::LFOChaseGradient()},
            {.column = 2, .slot = 3, .pattern = new ExamplePatterns::FadePattern()},
            {.column = 2, .slot = 4, .pattern = new ExamplePatterns::FadeChasePattern()},
            {.column = 2, .slot = 5, .pattern = new ExamplePatterns::FadeFinishPattern()},
            {.column = 2, .slot = 6, .pattern = new ExamplePatterns::TransitionPattern()},
            {.column = 2, .slot = 7, .pattern = new ExamplePatterns::PermutePattern()},

            {.column = 3, .slot = 0, .pattern = new ExamplePatterns::ParamsPattern()},

            {.column = 4, .slot = 0, .pattern = new ExamplePatterns::MappedPattern(haloMap)},
        });

    hyp->addPipe(new ConvertPipe<RGBA, RGB>(
        input,
        new MonitorOutput(haloMap)));
}

void addPaletteColumn(Hyperion *hyp)
{
    auto paletteColumn = new PaletteColumn(
        &hyp->hub,  //The control hub to connect to
        0,          //Column index (0 based)
        {           //The list of palettes
            heatmap,
            sunset1,
        });
    hyp->hub.subscribe(paletteColumn);
}
