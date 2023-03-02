#pragma once

#include "input.hpp"
#include "platform/includes/socket.hpp"
#include "platform/includes/log.hpp"

// UdpInput opens an UDP port, and listens for incoming packages. Each package
// will correspond to 1 new frame. I enabled IP reassembly in sdkconfig.defaults,
// and that seems to work for 2 packets, but not more than that. For this reason
// UDP input is limited to 975  RGB leds/port. More than enough for now
class UDPInput : public Input
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

    virtual int loadData(uint8_t *dataPtr, unsigned int buffersize) override
    {
        int cb, cbr;
        bool gotFrame = false;
        // read all packets, throw them away, but keep the last one.
        while ((cbr = sock->recv(dataPtr, buffersize)) > 0)
        {
            gotFrame = true;
            cb = cbr;
            missedframecount++;
        }
        if (!gotFrame)
            return 0;

        usedframecount++;
        missedframecount--;
        return cb;
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