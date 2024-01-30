#pragma once
#include "abstractTempo.h"
#include "log.hpp"
#include "socket.hpp"
#include "utils.hpp"

#define PRO_DJ_LINK_PORT_KEEP_ALIVE 50000
#define PRO_DJ_LINK_PORT_SYNC 50001
#define PRO_DJ_LINK_PORT_STATUS 50002

#define PACKET_TYPE_INDICATOR 0x0a
#define DEVICE_IS_MASTER_INDICATOR 0x9e

#define PACKET_TYPE_BEAT 0x28
#define PACKET_TYPE_STATUS 0x0a

#define DEVICE_ID 0x21
#define BEAT_NUMBER 0x5c

// ProDJLinkTempo will connect to Pioneer cdj/xdj's and sync to their beats.
// only works if the tracks are analyzed with Rekordbox.

// https://djl-analysis.deepsymmetry.org/djl-analysis/beats.html
class ProDJLinkTempo : public AbstractTempo
{
private:
    static ProDJLinkTempo *instance;

public:
    ProDJLinkTempo()
    {
        sourceName = "ProDJLink";
    }

    static ProDJLinkTempo *getInstance()
    {
        if (!instance)
            instance = new ProDJLinkTempo();
        return instance;
    }

private:
    Socket syncSocket = Socket(PRO_DJ_LINK_PORT_SYNC);
    Socket statusSocket = Socket(PRO_DJ_LINK_PORT_STATUS);
    Socket keepAliveSocket = Socket(PRO_DJ_LINK_PORT_KEEP_ALIVE);
    unsigned long lastKeepAlive = 0; // when the last keep alive was sent to the other players
    int master = -1;                 // the cd player that is currently tempo master
    uint8_t buffer[1500];            // newer cdj versions tend to send longer and longer packets, allocate enough buffer for forward compatibility
    const char proDjLinkHeader[10] = {0x51, 0x73, 0x70, 0x74, 0x31, 0x57, 0x6d, 0x4a, 0x4f, 0x4c};

    bool isProDjLink(uint8_t *message)
    {
        return strncmp((char *)message, proDjLinkHeader, 10) == 0;
    }

    void TempoTask()
    {
        int len;
        while ((len = syncSocket.receive(buffer, sizeof(buffer))))
        {
            if (!(isProDjLink(buffer) && buffer[PACKET_TYPE_INDICATOR] == PACKET_TYPE_BEAT))
                continue;

            int device = buffer[DEVICE_ID];
            int beatNumber = buffer[BEAT_NUMBER];

            // if no master is set yet, make the cdj that sends the first beat the master
            // (except if beatNumber has value 16, which is a message all devices send on startup)
            if (master == -1 && beatNumber != 16)
                master = device;

            if (device == master)
            {
                beat();

                // adjust the beat counter if it is not in sync with the beat counter of the cdj anymore.
                int beatMod4cdj = beatNumber - 1;   // beat number 0-3 from the cdj
                int beatMod4local = beatNumber % 4; // beat number 0-3 from this Tempo class
                int diff = beatMod4cdj - beatMod4local;
                if (diff != 0)
                {
                    if (diff == -2)
                        diff = 2; // prefer skipping +2 over -2
                    else if (diff == -3)
                        diff = 1; // adjust skips to the nearest match
                    else if (diff == 3)
                        diff = -1; // adjust skips to the nearest match

                    beatNumber += diff;
                }
            }
            validSignal = true;
            // Log::info(TAG,"len=%d, id=%d, beat=%d, device=%d, master=%d\r\n", len, buffer[0x21], buffer[0x5c], buffer[0x5f], master);
        }

        while ((len = statusSocket.receive(buffer, sizeof(buffer))))
        {
            // Log::info(TAG, "status %x, %x\r\n", len, buffer[PACKET_TYPE_INDICATOR]);

            if (!(isProDjLink(buffer) && buffer[PACKET_TYPE_INDICATOR] == PACKET_TYPE_STATUS))
                continue;

            int device = buffer[DEVICE_ID];
            bool isMaster = buffer[DEVICE_IS_MASTER_INDICATOR] > 0;
            if (isMaster && master != device)
            {
                master = device;
                Log::info(TAG, "Master handoff to %d", device);
            }
        }

        while ((len = keepAliveSocket.receive(buffer, sizeof(buffer))))
        {
            // Log::info(TAG,"got keep alive %x, %x\r\n", len, buffer[0x0a]);
        }

        if (Utils::millis() - lastKeepAlive > 300 && Ethernet::isConnected())
            SendKeepAlive();
    }

    void SendKeepAlive()
    {
        uint8_t device_id = 5;
        uint8_t buffer[] = {
            // proDJLink header
            0x51, 0x73, 0x70, 0x74, 0x31, 0x57, 0x6d, 0x4a, 0x4f, 0x4c,

            // keep alive packet type indicator
            0x06, 0x00,

            // device name
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

            // packet structure?
            1, 2,

            // packet length
            0, 0x36,

            device_id,

            //?
            01,

            // mac
            0, 0, 0, 0, 0, 0,

            // ip
            0, 0, 0, 0,

            // ?
            1, 0, 0, 0, 1, 0};

        uint32_t ip = Ethernet::getIp().toUint32();

        memcpy(buffer + 0x26, Ethernet::getMac().octets, 6);
        memcpy(buffer + 0x2c, &ip, 4);

        auto broadcastIp = IPAddress::broadcast();
        statusSocket.send(
            &broadcastIp,
            PRO_DJ_LINK_PORT_KEEP_ALIVE,
            buffer,
            sizeof(buffer));

        lastKeepAlive = Utils::millis();

        // Log::info(TAG,"send keepalive");
    }
};

ProDJLinkTempo *ProDJLinkTempo::instance = nullptr;