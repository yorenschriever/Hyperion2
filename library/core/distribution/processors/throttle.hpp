
#pragma once

#include "../interfaces.hpp"
#include "platform/includes/utils.hpp"
#include "platform/includes/neoPixels.hpp"

class Throttle final:  public IProcessor
{
private:
    int byteTimeNs = 0;
    int resetTimeNs = 0;
    unsigned long lastFrame = 0;
    unsigned long lastFrameUs = 0;
    unsigned long frameInterval = 0;
    int dataLength = 0;

public: 
    /*
    * Throttle will limit the data rate to a certain kbps and fps.
    * Both limitations must be satisfied, so the slowest one will determine the actual data rate.
    * This is useful for udpoutputs that send led data that is later sent out to the leds at a 
    * specific data rate. By Matching the frame rate on host already, no frames will be thrown away
    * on the hypernode, which means that strobe animations will be smoother, latency will be shorter
    * cpu usage will be lower and fps analytics will be more accurate.
     */
    Throttle(NeoPixels::Timing timing = NeoPixels::Kpbs800, int fps = 120)
    {
        int nsPerBit = std::max(timing.bit0_high + timing.bit0_low , timing.bit1_high + timing.bit1_low) ; 
        this->byteTimeNs = nsPerBit * 8; // time per byte in ns
        this->resetTimeNs = timing.reset; // reset time in ns
        this->frameInterval = 1000 / fps;
    }

    void initialize() override {}

    bool ready() override
    {
        bool fpsReady = (Utils::millis() - lastFrame >= frameInterval);

        unsigned long dataDurationUs = (dataLength * byteTimeNs + resetTimeNs) / 1000; // time in us that it should take to send the data at the specified kbps
        bool kbpsReady = (Utils::micros() - lastFrameUs >= dataDurationUs);

        return fpsReady && kbpsReady;   
    }

    const Buffer process(const Buffer& inputBuffer) override
    {
        lastFrame = Utils::millis();
        lastFrameUs = Utils::micros();
        dataLength = inputBuffer.size();
        return inputBuffer;
    }
    
};
