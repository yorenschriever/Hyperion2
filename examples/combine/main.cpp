#include "hyperion.hpp"
#include "patterns.hpp"

// This example will show you how to combine multiple chains.
// This is useful if you want to send data to multiple fixture types
// on a single DMX port on a hypernode.

int main()
{
    auto hyp = new Hyperion();

    // This will yield 9 monochrome pixels
    int numMonochromePixels = 9;
    auto inputBulbs = new ControlHubInput<Monochrome>(
        numMonochromePixels,
        &hyp->hub,
        {
            {.column = 0, .slot = 0, .pattern = new MonochromePatterns::SinPattern()},
            {.column = 0, .slot = 1, .pattern = new MonochromePatterns::GlowPattern()},
        });

    // This wil yield 3 RGBA pixels
    int numRGBAPixels = 3;
    auto inputLeds = new ControlHubInput<RGBA>(
        numRGBAPixels,
        &hyp->hub,
        {
            {.column = 1, .slot = 0, .pattern = new LedPatterns::OnPattern(RGB(255, 0, 0), "Red")},
            {.column = 1, .slot = 1, .pattern = new LedPatterns::OnPattern(RGB(0, 255, 0), "Green")},
            {.column = 1, .slot = 2, .pattern = new LedPatterns::OnPattern(RGB(0, 0, 255), "Blue")},
            {.column = 1, .slot = 3, .pattern = new LedPatterns::OnPattern(RGB(255, 255, 255), "White")},
            {.column = 1, .slot = 4, .pattern = new LedPatterns::GlowPattern()},
        });

    // Here the inputs are combined into a single input.
    // The monochrome channels are placed at the start of the buffer
    // Then a gap of 3 channels is left (for illustration purposes)
    // Finally, the RGBA channels are placed at the end of the buffer.
    // But, because our lamps are RGB, and the controlhub outputs RGBA,
    // we need to convert the RGBA to RGB.

    auto combined = new Combine();
    hyp->createChain(
        inputBulbs,
        combined->atOffset(0)
    );
    hyp->createChain(
        inputLeds,
        new ColorConverter<RGBA, RGB>(),
        combined->atOffset(numMonochromePixels * sizeof(Monochrome) + 3)
    );

    // The chain below will send the data to the DMX output on a hypernode
    // This is the most likely use case, but demo purpose i send the combined data to
    // a monitorOutput below

    // hyp->createChain(
    //     combined,
    //     new UDPOutput("hypernode1.local",9619,60)
    // );

    // Create a grid map of 3x7 channels. The top 3 rows will contain data for the monochrome bulbs.
    // There will be 1 row spacing, and
    // the bottom 3 rows will contain RGB led data
    auto map = gridMap(3, 7, 0.2);

    // This chain will send the data to the monitor, so you can see the data in your browser
    // without connecting any hardware.
    // In this demo, we want to see all channels rendered as individual dots on the screen.
    // The monitorOutput expects RGB values per dot, so we convert from monochrome to RGB
    hyp->createChain(
        combined,
        new ColorConverter<Monochrome, RGB>(),
        new MonitorOutput(&hyp->webServer, &map, 60, 0.03)
    );

    hyp->start();

    while (1)
        Thread::sleep(1000);
}
