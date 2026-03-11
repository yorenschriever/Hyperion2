
#pragma once

#include "../interfaces.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/utils.hpp"
#include "../buffer.hpp"
#include <string>
#include <algorithm>
/*
 * RepeatFallback will repeat the last used frame when the input stops.
 * This can be used on a hypernode to keep the lights on in case of a problem.
 * 
 * The fallbackTimeout parameter specifies the duration (in milliseconds) to wait
 * before activating the fallback. If the input does not provide a new frame within this time, 
 * the last frame will be repeated every fallbackTimeout interval until a new frame is received.
 * 
 * RepeatFallback is seen as a routing class, because it routes from 1 chain to another.
 * The input chain is for the informing data. This chain will not process when no new data is received,
 * So RepeatFallback needs a second chain in which it can trigger the new, repeated frames.
 * 
 * During normal operation, RepeatFallback copies over the pixel data from the input chain to the output chain.
 * After the timeout period, it will trigger repeated frames in output chain.
 * 
 * Usage:
 * auto repeatFallback = new RepeatFallback();
 * hyp->createChain(input, repeatFallback.sink); 
 * hyp->createChain(repeatFallback.source, output);
 * 
 */
class RepeatFallback final
{
    unsigned long lastFrameTime = 0;
    Buffer lastBuffer = Buffer(0);
    int fallbackTimeout;
    bool frameReady = false;

    class Sink : public ISink
    {
        friend RepeatFallback;
        RepeatFallback *parent;
        Sink (RepeatFallback *parent) : parent(parent) {}
        void initialize() override { }
        bool ready() override { return !parent->frameReady; }
        void process(const Buffer& inputBuffer) override { 
            parent->lastFrameTime = Utils::millis();
            parent->lastBuffer = inputBuffer;
            parent->frameReady = true;
        }
    };

    class Source : public ISource
    {
        friend RepeatFallback;
        RepeatFallback *parent;
        Source (RepeatFallback *parent) : parent(parent) {}
        void initialize() override { }
        bool ready() override { 
            if (parent->frameReady)
                return true;

            return Utils::millis() - parent->lastFrameTime >= parent->fallbackTimeout;
        }
        const Buffer process() override { 
            parent->frameReady = false;
            return parent->lastBuffer;
        }
    };

    Sink _sink = Sink(this);
    Source _source = Source(this);

public: 
    Source *source = &_source;
    Sink *sink = &_sink;

    RepeatFallback(int fallbackTimeout = 250)
    {
        this->fallbackTimeout = fallbackTimeout;
    }
};

