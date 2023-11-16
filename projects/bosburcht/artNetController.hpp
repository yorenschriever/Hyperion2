#pragma once
#include "artNet.hpp"
#include "core/generation/controlHub/controlHub.hpp"
#include "generation/controlHub/paletteColumn.hpp"
#include "thread.hpp"

class ArtNetController
{

public:
    void subscribe(ControlHub *hub, int universe)
    {
        this->hub = hub;
        this->universe = ArtNet::getInstance()->addUniverse(universe);
        this->universe->subscribe(onFrameCallback, this);
        //Thread::create(handleFrameTask, "artNetController",Thread::control,4096,this,6);
    }

    void linkToPalette(Palette *palette)
    {
        this->palette = palette;
    }

private:
    ControlHub *hub;
    ArtNetUniverse *universe;
    uint8_t lastFrame[512];
    const int channelsPerColumn = 25;
    Palette *palette = nullptr;
    // std::binary_semaphore frameToProcess{0};
    const char * TAG  ="Artnet";
    uint8_t sequence;

    static void onFrameCallback(void *params)
    {
        auto instance = (ArtNetController *)(params);
        //instance->frameToProcess.release();
        instance->handleFrame();
    }

    inline bool changed(int channel)
    {
        return lastFrame[channel] != ch(channel);
    }

    inline bool changedRange(int startChannel, int endChannel)
    {
        for (int i = startChannel; i < endChannel; i++)
            if (changed(i))
                return true;
        return false;
    }

    // static void handleFrameTask(void* params)
    // {
    //     auto instance = (ArtNetController *)(params);
    //     while(true)
    //     {
    //         instance->frameToProcess.acquire();
    //         instance->handleFrame();
    //     }
    // }

    bool sequenceNumberIncreased()
    {
        if (uint8_t(universe->sequence - sequence) > 200)
            return false;
        sequence = universe->sequence;
        return true;
    }

    inline uint8_t ch(int channelIndex)
    {
        return universe->channels[channelIndex];
    }

    void handleFrame()
    {
        if (!sequenceNumberIncreased())
            return;

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
                hub->dim(col, ch(channelIndex));
            }

            for (int slot = 0; slot < hub->getSlotCount(col); slot++)
            {
                channelIndex++;
                if (changed(channelIndex))
                {
                    hub->setSlotActive(col, slot, ch(channelIndex) > 0);
                }
            }
        }

        // be aware that DMX channel are 1-indexed, but the channels array is zero indexed,
        // so velocity is dmx channel 501
        if (changed(500))
            hub->setVelocity(0, ch(500));
        if (changed(501))
            hub->setAmount(0, ch(501));
        if (changed(502))
            hub->setSize(0, ch(502));
        if (changed(503))
            hub->setOffset(0, ch(503));
        if (changed(504))
            hub->setVariant(0, ch(504));
        if (changed(505))
            hub->setIntensity(0, ch(505));
        if (changed(511))
            hub->setMasterDim(ch(511));

        // palette change
        if (!palette)
            return;
        const int paletteStartChannel = 400;
        if (changedRange(paletteStartChannel, paletteStartChannel + 3))
        {
            // Log::info(TAG,"Change primary");
            palette->primary = RGB(
                ch(paletteStartChannel + 0),
                ch(paletteStartChannel + 1),
                ch(paletteStartChannel + 2));
        }
        if (changedRange(paletteStartChannel + 3, paletteStartChannel + 6))
        {
            // Log::info(TAG,"Change secondary");
            palette->secondary = RGB(
                ch(paletteStartChannel + 3),
                ch(paletteStartChannel + 4),
                ch(paletteStartChannel + 5));
        }
        if (changedRange(paletteStartChannel, paletteStartChannel + 9))
        {
            // Log::info(TAG,"Change gradient");

            int totalWidth = ch(paletteStartChannel + 6) + ch(paletteStartChannel + 7) + ch(paletteStartChannel + 8);
            uint8_t pos1,pos2;
            if (totalWidth>0){
                int width1Norm = 255 * ch(paletteStartChannel + 6) / totalWidth;
                int width2Norm = 255 * ch(paletteStartChannel + 7) / totalWidth;

                // int endpos = ch(paletteStartChannel + 8);
                // pos1 = endpos * ch(paletteStartChannel + 6) / totalWidth;
                // pos2 = pos1 + endpos * ch(paletteStartChannel + 7) / totalWidth; 
                pos1 = 255 - width1Norm- width2Norm;
                pos2 = 255 - width1Norm ;
            } else{
                pos1=128; pos2=224; 
            }

            palette->gradient = Gradient({
                {.position = 0, .color = RGB(0, 0, 0)},         // Darkest
                {.position = pos1, .color = palette->secondary}, // ..
                {.position = pos2, .color = palette->primary},   // ..
                {.position = 255, .color = palette->highlight}  // Brightest
            });
        }

        memcpy(lastFrame, universe->channels, 512);
    }
};