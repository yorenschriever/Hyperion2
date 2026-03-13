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

    virtual void initialize() override
    {
        sock = new Socket(this->port, AF_INET, 0, false, 2 * MTU + 200);
    }

    virtual bool ready() override
    {
        do {
            int sz = sock->receive(packetBuffer, packetBufferSize);
            if (sz <= 0) break;
            bufferSize = sz;
            gotFrame = true;
        } while (1);

        return gotFrame;
    }

    const Buffer process() override 
    {
        if (!gotFrame)
            return Buffer(0);
        gotFrame = false;

        auto result = Buffer(bufferSize);
        memcpy(result.data(), packetBuffer, bufferSize);
        return result;
    }

    ~UDPInput()
    {
        if (sock)
            delete sock;
    }

private:
    int port;
    Socket *sock = NULL;
    static const int packetBufferSize = 2 * MTU;
    uint8_t packetBuffer[packetBufferSize];
    bool gotFrame = false;
    int bufferSize=0;
};