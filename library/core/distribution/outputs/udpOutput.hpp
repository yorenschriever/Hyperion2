#pragma once

#include "output.hpp"
#include "core/distribution/utils/hostnameCache.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/socket.hpp"
#include "platform/includes/thread.hpp"
#include "platform/includes/utils.hpp"
#include <algorithm>
#include "log.hpp"

// UDPoutput writes led data to another device over UDP.
class UDPOutput : public Output
{
public:
    UDPOutput(const char *hostname, int port, unsigned int fps)
    {
        this->hostname = hostname;
        this->port = port;
        this->frameInterval = 1000 / fps;

        this->length = 12;
        buffer = (uint8_t *)malloc(12);
    }

    // index and size are in bytes
    void setData(uint8_t *data, int size, int index) override
    {
        int copy_length = std::min(size, length - index);
        if (copy_length > 0)
            memcpy(this->buffer + index, data, copy_length);
    }

    bool ready() override
    {
        return (Utils::millis() - lastFrame >= frameInterval);
    }

    void show() override
    {
        lastFrame = Utils::millis();
        sock->send(HostnameCache::lookup(hostname), port, buffer, length);
    }

    void postProcess() override
    {
    }

    void begin() override
    {
        lastFrame = Utils::millis();
        if (sock == NULL)
            sock = new Socket();
    }

    void clear() override
    {
        memset(this->buffer, 0, this->length);
    }

    // length is in bytes
    void setLength(int len) override
    {
        if (len != this->length)
        {
            if (len > 1500){
                Log::info("UPD_OUTPUT","Warning! udp output length is > 1500, which is the maximum transfer length on some systems. Data might not be sent.");
            }

            // wait for the front buffer to be sent before we are going to change its size
            while (!ready())
                Thread::sleep(1);

            buffer = (uint8_t *)realloc(buffer, len);

            if (!buffer)
            {
                Log::error("UPD_OUTPUT", "Unable to allocate memory for UdpOutput, free heap = %d\n", Utils::get_free_heap());
                Utils::exit();
            }

            this->length = len;
        }
    }

    ~UDPOutput()
    {
        if (sock != NULL)
            delete sock;
    }
protected:
    const char *hostname;
    int port;
    int frameInterval;
    unsigned long lastFrame = 0;

    int length;
    uint8_t *buffer;

    // share 1 socket instance with all UdpOutputs, because the number of sockets is limited on esp platforms
    static Socket *sock;
};

Socket *UDPOutput::sock;