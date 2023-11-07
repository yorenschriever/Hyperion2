#pragma once
#include "artNet.hpp"
#include "core/generation/controlHub/controlHub.hpp"
#include "generation/controlHub/paletteColumn.hpp"

class ArtNetController
{

public:
    void subscribe(ControlHub *hub, int universe)
    {
        this->hub = hub;
        this->universe = ArtNet::getInstance()->addUniverse(universe);
        this->universe->subscribe(onFrameCallback, this);
    }

    void linkToPalette(Palette *palette)
    {
        this->palette = palette;
    }

private:
    ControlHub *hub;
    ArtNetUniverse *universe;
    uint8_t lastFrame[512];
    const int channelsPerColumn = 50;
    Palette *palette = nullptr;

    static void onFrameCallback(void *params)
    {
        auto instance = (ArtNetController *)(params);
        instance->handleFrame();
    }

    inline bool changed(int channel)
    {
        return lastFrame[channel] != universe->channels[channel];
    }

    inline bool changedRange(int startChannel, int endChannel)
    {
        for (int i = startChannel; i < endChannel; i++)
            if (changed(i))
                return true;
        return false;
    }

    void handleFrame()
    {
        for (int col = 0; col < hub->getColumnCount(); col++)
        {
            if (col==0) continue; //ignore palette column for now.

            // Each column will start with an address for the alpha, and then 19 channels for the patterns.
            // ie dmx channel 1 will be fader for column 0
            //    dmx channel 2 will be the first pattern in column 1 etc.
            //  be aware that dmx is 1-based, while the channels arrays is 0-based.
            int channelIndex = col * 20;
            if (changed(channelIndex))
            {
                hub->dim(col, universe->channels[channelIndex]);
            }

            for (int slot = 0; slot < hub->getSlotCount(col); slot++)
            {
                channelIndex++;
                if (changed(channelIndex))
                {
                    hub->setSlotActive(col, slot, universe->channels[channelIndex] > 0);
                }
            }
        }

        // be aware that DMX channel are 1-indexed, but the channels array is zero indexed,
        // so velocity is dmx channel 501
        if (changed(500))
            hub->setVelocity(0, universe->channels[500]);
        if (changed(501))
            hub->setAmount(0, universe->channels[501]);
        if (changed(502))
            hub->setSize(0, universe->channels[502]);
        if (changed(503))
            hub->setOffset(0, universe->channels[503]);
        if (changed(504))
            hub->setVariant(0, universe->channels[504]);
        if (changed(505))
            hub->setIntensity(0, universe->channels[505]);
        if (changed(511))
            hub->setMasterDim(universe->channels[511]);

        // palette change
        if (!palette)
            return;
        const int paletteStartChannel = 0;
        if (changedRange(paletteStartChannel, paletteStartChannel + 3))
        {
            Log::info("Artnet","Change primary");
            palette->primary = RGB(
                universe->channels[paletteStartChannel + 0],
                universe->channels[paletteStartChannel + 1],
                universe->channels[paletteStartChannel + 2]);
        }
        if (changedRange(paletteStartChannel + 3, paletteStartChannel + 6))
            palette->secondary = RGB(
                universe->channels[paletteStartChannel + 3],
                universe->channels[paletteStartChannel + 4],
                universe->channels[paletteStartChannel + 5]);
        if (changedRange(paletteStartChannel, paletteStartChannel + 6))
        {
            Log::info("Artnet","Change gradient");
            palette->gradient = Gradient({
                {.position = 0, .color = RGB(0, 0, 0)},         // Darkest
                {.position = 128, .color = palette->secondary}, // ..
                {.position = 224, .color = palette->primary},   // ..
                {.position = 255, .color = palette->highlight}  // Brightest
            });
        }

        memcpy(lastFrame, universe->channels, 512);
    }
};