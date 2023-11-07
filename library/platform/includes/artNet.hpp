#pragma once

#include "ethernet.hpp"
#include "network.hpp"
#include "socket.hpp"
#include "thread.hpp"
#include <map>

class ArtNetUniverse
{
public:
    using OnFrameCallback = void (*)(void *userData);
    typedef struct {OnFrameCallback callback; void* userData;} Subscription;

    int port = 0;
    uint8_t channels[512];
    int size = 0;
    uint8_t sequence=0;
    void subscribe(OnFrameCallback callback, void* userData) {
        subscriptions.push_back({callback, userData});
    }

    void notifyFrame()
    {
        for (auto subscription : subscriptions)
            subscription.callback(subscription.userData);
    }

private: 
    std::vector<Subscription> subscriptions;
};

class ArtNet
{
public:
    static ArtNet *getInstance()
    {
        if (!instance)
            instance = createInstance();
        return instance;
    }

    virtual ArtNetUniverse *addUniverse(uint16_t port)=0;

    virtual ~ArtNet() = default;

protected:
    static ArtNet *instance;
    static ArtNet *createInstance();
    ArtNet() {}
};

#define ART_NET_PORT 6454
// Opcodes
#define ART_POLL 0x2000
#define ART_POLL_REPLY 0x2100
#define ART_DMX 0x5000
#define ART_SYNC 0x5200
// Buffers
#define MAX_BUFFER_ARTNET 530
// Packet
#define ART_NET_ID "Art-Net\0"
#define ART_DMX_START 18

struct artnet_reply_s
{
    uint8_t id[8];
    uint16_t opCode;
    uint8_t ip[4];
    uint16_t port;
    uint8_t verH;
    uint8_t ver;
    uint8_t subH;
    uint8_t sub;
    uint8_t oemH;
    uint8_t oem;
    uint8_t ubea;
    uint8_t status;
    uint8_t etsaman[2];
    uint8_t shortname[18];
    uint8_t longname[64];
    uint8_t nodereport[64];
    uint8_t numbportsH;
    uint8_t numbports;
    uint8_t porttypes[4]; // max of 4 ports per node
    uint8_t goodinput[4];
    uint8_t goodoutput[4];
    uint8_t swin[4];
    uint8_t swout[4];
    uint8_t swvideo;
    uint8_t swmacro;
    uint8_t swremote;
    uint8_t sp1;
    uint8_t sp2;
    uint8_t sp3;
    uint8_t style;
    uint8_t mac[6];
    uint8_t bindip[4];
    uint8_t bindindex;
    uint8_t status2;
    uint8_t filler[26];
} __attribute__((packed));

class GenericArtNet : public ArtNet
{
private:
    const char *TAG = "ArtNet";
    bool initialized = false;
    Socket sock = Socket(ART_NET_PORT, AF_INET, 10*1000);
    std::map<int, ArtNetUniverse> universes;
    uint8_t recvBuffer[MAX_BUFFER_ARTNET];

public:
    GenericArtNet()
    {
        initialize();
    }

    ~GenericArtNet() = default;

    void initialize()
    {
        if (initialized)
            return;
        initialized = true;

        Ethernet::initialize();

        Thread::create(ArtNetListenTask, "artnet", Thread::control, 4096, this, 6);
    }

    ArtNetUniverse *addUniverse(uint16_t port) override {
        ArtNetUniverse uni;
        uni.port = port;
        if (universes.find(port) == universes.end())
            // universes.insert({port, ArtNetUniverse{.port=port}});
            universes.insert({port, uni});
        
        return &(universes.find(port)->second);
    }

private:
    static void ArtNetListenTask(void *params)
    {
        auto instance = (GenericArtNet *)params;

        while (true)
        {
            int size = instance->sock.receive_wait(instance->recvBuffer, MAX_BUFFER_ARTNET);
            if (size > 0)
                instance->parsePacket(size);
        }
    }

    void parsePacket(int size)
    {
        if (strncmp((char *)&recvBuffer, ART_NET_ID, sizeof(ART_NET_ID)) != 0)
            return;

        uint16_t opcode = opcode = recvBuffer[8] | recvBuffer[9] << 8;
        auto ipAddress = Ethernet::getIp();

        if (opcode == ART_DMX)
        {
            uint8_t sequence = recvBuffer[12];
            uint16_t incomingUniverse = recvBuffer[14] | recvBuffer[15] << 8;
            uint16_t dmxDataLength = recvBuffer[17] | recvBuffer[16] << 8;

            // Log::info(TAG, "Got artnet DMX Packet, universe=%d, channel 0= %d, sequence=%d", incomingUniverse, recvBuffer[ART_DMX_START], sequence);
            
            if (auto universe = universes.find(incomingUniverse); universe != universes.end())
            {
                universe->second.size = dmxDataLength;
                universe->second.sequence = sequence;
                memcpy(universe->second.channels, recvBuffer + ART_DMX_START, dmxDataLength);
                universe->second.notifyFrame();
                return;
            }

            return;
        }

        if (opcode == ART_POLL)
        {
            // Log::info(TAG, "ArtNet Poll");
            uint32_t ipNetOrder = htonl(ipAddress.toUint32());

            artnet_reply_s reply;
            memcpy(reply.id, ART_NET_ID, sizeof(ART_NET_ID));
            memcpy(reply.ip, &ipNetOrder, 4);

            reply.opCode = ART_POLL_REPLY;
            reply.port = ART_NET_PORT;

            memset(reply.goodinput, 0x08, 4);
            memset(reply.goodoutput, 0x80, 4);
            memset(reply.porttypes, 0xc0, 4);

            snprintf((char *)reply.shortname, 18, "Hyperion ArtNet");
            snprintf((char *)reply.longname, 64, "Hyperion ArtNet" /*, Network::GetHostName()*/);

            reply.etsaman[0] = 0;
            reply.etsaman[1] = 0;
            reply.verH = 1;
            reply.ver = 0;
            reply.subH = 0;
            reply.sub = 0;
            reply.oemH = 0;
            reply.oem = 0xFF;
            reply.ubea = 0;
            reply.status = 0xd2;
            reply.swvideo = 0;
            reply.swmacro = 0;
            reply.swremote = 0;
            reply.style = 0;

            reply.numbportsH = 0;
            reply.numbports = 4;
            reply.status2 = 0x08;

            memcpy(&reply.bindip, &ipNetOrder, 4);

            uint8_t swin[4] = {0x01, 0x02, 0x03, 0x04};
            uint8_t swout[4] = {0x01, 0x02, 0x03, 0x04};
            for (uint8_t i = 0; i < 4; i++)
            {
                reply.swout[i] = swout[i];
                reply.swin[i] = swin[i];
            }

            auto broadcast = IPAddress::broadcast();
            sock.send(&broadcast, ART_NET_PORT, (uint8_t *)&reply, sizeof(reply));
        }
    }
};

ArtNet *ArtNet::createInstance() { return new GenericArtNet(); }
ArtNet *ArtNet::instance = nullptr;