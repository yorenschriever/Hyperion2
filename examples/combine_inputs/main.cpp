#include "gridmap.hpp"
#include "hyperion.hpp"
#include "patterns.hpp"

// This example will show you how to combine multiple inputs.
// This is useful if you want to send data fox multiple fixtures
// to a single DMX port on a hypernode.

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
            {.column = 1, .slot = 2, .pattern = new LedPatterns::OnPattern(RGB(255, 255, 255), "White")},
            {.column = 1, .slot = 3, .pattern = new LedPatterns::GlowPattern()},
        });

    // Here the inputs are combined into a single input.
    // The monochrome channels are placed at the start of the buffer
    // Then a gap of 3 channels is left (for illustration purposes)
    // Finally, the RGBA channels are placed at the end of the buffer.
    // But, because our lamps are RGB, and the controlhub outputs RGBA,
    // we need to convert the RGBA to RGB.
    //   auto combined = new CombinedInput(
    //       {
    //           {.input = inputBulbs, .offset = 0},
    //           {.input = inputLeds, .offset = numMonochromePixels * (int)sizeof(Monochrome) + 3, .convert = CombinedInput::convert<RGBA, RGB>},
    //       },
    //       numMonochromePixels * sizeof(Monochrome) + 3 + numRGBAPixels * sizeof(RGB));

    // This pipe will send the data to the DMX output
    // This is the most likely use case, but demo purpose i send the combined data to
    // a monitorOutput below
    // auto pipe = new Pipe(
    //   combined,
    //   new UDPOutput("hypernode1.local",9621,60)
    // );

    // This pipe will send the data to the monitor, so you can see the data in your browser
    // without connecting any hardware.
    // Create a grid map of 3x7 channels. The top 3 rows will be monochrome, the bottom 3 rows will be RGB
    auto map = gridMap(3, 7, 0.2);
    // In this demo, we want to see all channels rendered as individual pixels.
    // The monitorOutput expects RGB values per pixel, so we convert from monochrome to RGB

    auto combined = new CombinedInput();
    //inputBulbs->setReceiver(combined->createOutput(0));
    inputLeds->setReceiver(
        (new ConvertColor<RGBA, RGB>())->setReceiver(
            combined->createOutput(numMonochromePixels * sizeof(Monochrome) + 3)
        )
    );

    combined->setReceiver(
        (new ConvertColor<Monochrome, RGB>())->setReceiver(new MonitorOutput(&hyp->webServer, &map, 60, 0.03)));

    //hyp->addInput(inputBulbs);
    hyp->addInput(inputLeds);

    // inputBulbs->setOutput(
    //         new MonitorOutput(&hyp->webServer, &map, 60, 0.03));

    // inputBulbs->setOutput(
    //     (new ConvertColor<Monochrome,RGB>())->setOutput(
    //         new MonitorOutput(&hyp->webServer, &map, 60, 0.03)
    //     )
    // );
    // hyp->addInput(inputBulbs);

    //   auto pipe = new ConvertPipe<Monochrome, RGB>(
    //       combined,
    //       new MonitorOutput(&hyp->webServer, &map, 60, 0.03));

    //   hyp->addPipe(pipe);

    // inputLeds->setReceiver((new ConvertColor<RGBA, RGB>())->setReceiver(new MonitorOutput(&hyp->webServer, &map, 60, 0.03)));
    // hyp->addInput(inputLeds);

    hyp->start();

    while (1)
        Thread::sleep(1000);
}
