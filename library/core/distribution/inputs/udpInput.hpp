#pragma once

#include "baseInput.hpp"
#include "platform/includes/socket.hpp"
#include "platform/includes/log.hpp"

const int MTU = 1400;

// UdpInput opens an UDP port, and listens for incoming packages. Each package
// will correspond to 1 new frame. I enabled IP reassembly in sdkconfig.defaults,
// and that seems to work for 2 packets, but not more than that. For this reason
// UDP input is limited to 975  RGB leds/port. More than enough for now
class UDPInput final: public BaseInput
{

public:
    UDPInput(int port)
    {
        this->port = port;
    }

    virtual void begin() override
    {
        sock = new Socket(this->port);
    }

    Buffer *getData() override 
    {
        const int bufferSize = 2 * MTU;
        auto patternBuffer = BufferPool::getBuffer(bufferSize);
        if (!patternBuffer)
        {
            Log::error("PATTERN_INPUT", "Unable to allocate memory for PatternInput, free heap = %d\n", Utils::get_free_heap());
            Utils::exit();
        }
        auto dataPtr = patternBuffer->getData();

        bool gotFrame = false;
        int missedFrameCount = -1;
        // read all packets, throw them away, but keep the last one.
        while ((sock->receive(dataPtr, bufferSize)) > 0)
        {
            gotFrame = true;
            missedFrameCount++;
        }

        if (!gotFrame)
        {
            BufferPool::release(patternBuffer);
            return nullptr;
        }

        fpsCounter.increaseUsedFrameCount();

        if (missedFrameCount >= 1) 
        {
            //-1 means no frame waiting, 
            //0 means 1 frame was waiting and is used. 
            //>= 1 means we skipped frames
            fpsCounter.increaseMissedFrameCount(missedFrameCount);
        }
        
        return patternBuffer;
    }

    ~UDPInput()
    {
        if (sock)
            delete sock;
    }

private:
    int port;
    Socket *sock = NULL;
};