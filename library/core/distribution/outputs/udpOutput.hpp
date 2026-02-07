#pragma once

#include "baseOutput.hpp"
#include "core/distribution/utils/hostnameCache.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/socket.hpp"
#include "platform/includes/thread.hpp"
#include "platform/includes/utils.hpp"
#include <algorithm>
#include "log.hpp"

// UDPoutput writes led data to another device over UDP.
class UDPOutput final: public BaseOutput
{
public:
    UDPOutput(const char *hostname, int port, unsigned int fps = 60)
    {
        this->hostname = hostname;
        this->port = port;
        this->frameInterval = 1000 / fps;
    }

    bool ready() override
    {
        return (Utils::millis() - lastFrame >= frameInterval);
    }

    void process(const Buffer& inputBuffer) override
    {
        if (!ready())
            return;

        lastFrame = Utils::millis();

        sock->send(HostnameCache::lookup(hostname), port, (uint8_t *)inputBuffer.data(), inputBuffer.size());
    }

    void initialize() override
    {
        lastFrame = Utils::millis();
        if (sock == NULL)
            sock = new Socket();
    }

    ~UDPOutput()
    {
        // this wont work if the sock is shared with all instances..
        // if (sock != NULL)
        //     delete sock;
    }
protected:
    const char *hostname;
    int port;
    int frameInterval;
    unsigned long lastFrame = 0;


    // share 1 socket instance with all UdpOutputs, because the number of sockets is limited on esp platforms
    static Socket *sock;
};

Socket *UDPOutput::sock;