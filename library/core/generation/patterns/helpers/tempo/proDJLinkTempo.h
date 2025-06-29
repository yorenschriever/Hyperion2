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
#define PACKET_TYPE_KEEP_ALIVE 0x06

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
    Socket keepAliveSocket2 = Socket(0,AF_INET,5, true);
    unsigned long lastKeepAlive = 0; // when the last keep alive was sent to the other players
    int master = -1;                 // the cd player that is currently tempo master
    int lastbeat = -1;
    uint8_t buffer[1500];            // newer cdj versions tend to send longer and longer packets, allocate enough buffer for forward compatibility
    const char proDjLinkHeader[10] = {0x51, 0x73, 0x70, 0x74, 0x31, 0x57, 0x6d, 0x4a, 0x4f, 0x4c};
    bool syncWithCdj3000 = false; // whether to sync with cdj3000 players, which send a different keep alive packet


    bool isProDjLink(uint8_t *message)
    {
        return strncmp((char *)message, proDjLinkHeader, 10) == 0;
    }

    void TempoTask()
    {
        int len;
        while ((len = syncSocket.receive(buffer, sizeof(buffer)) > 0))
        {
            if (!(isProDjLink(buffer) && buffer[PACKET_TYPE_INDICATOR] == PACKET_TYPE_BEAT))
                continue;

            int device = buffer[DEVICE_ID];
            int beatNumberCdj = buffer[BEAT_NUMBER];

            // if no master is set yet, make the cdj that sends the first beat the master
            // (except if beatNumber has value 16, which is a message all devices send on startup)
            if (master == -1 && beatNumber != 16)
                master = device;

            if (device == master && lastbeat != beatNumber)
            {
                beat();
                lastbeat = beatNumber;
                Log::info(TAG,"len=%d, id=%d, beat=%d, device=%d, master=%d", len, buffer[0x21], buffer[0x5c], buffer[0x5f], master);

                // adjust the beat counter if it is not in sync with the beat counter of the cdj anymore.
                int beatMod4cdj = beatNumberCdj - 1;   // beat number 0-3 from the cdj
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
            // Log::info(TAG,"len=%d, id=%d, beat=%d, device=%d, master=%d", len, buffer[0x21], buffer[0x5c], buffer[0x5f], master);
        }

        while ((len = statusSocket.receive(buffer, sizeof(buffer)) > 0))
        {
            // Log::info(TAG, "status %x, %x", len, buffer[PACKET_TYPE_INDICATOR]);

            if (!(isProDjLink(buffer) && buffer[PACKET_TYPE_INDICATOR] == PACKET_TYPE_STATUS))
                continue;

            int device = buffer[DEVICE_ID];
            bool isMaster = buffer[DEVICE_IS_MASTER_INDICATOR] > 0;
            if (isMaster && master != device)
            {
                master = device;
                lastbeat = -1;
                Log::info(TAG, "Master handoff to %d", device);
            }
        }

        while ((len = keepAliveSocket.receive(buffer, sizeof(buffer)) > 0))
        {
            // Log::info(TAG,"got keep alive %x, %x\r\n", len, buffer[0x0a]);
            // Log::info(TAG,"received keepalive %d.%d.%d.%d", buffer[0x2c], buffer[0x2d],buffer[0x2e],buffer[0x2f])

            if (!(isProDjLink(buffer) && buffer[PACKET_TYPE_INDICATOR] == PACKET_TYPE_KEEP_ALIVE))
                continue;

            int device = buffer[DEVICE_ID];
            int deviceIsCdj3000 = buffer[0x35] > 0; // cdj3000 sends a different keep alive packet)

            if (deviceIsCdj3000 && !this->syncWithCdj3000)
            {
                this->syncWithCdj3000 = true;
                Log::info(TAG, "player %d detected as cdj3000. Changing keep alive packet structure", device);
            }
        }

        // Log::info(TAG,"keep alive? %lu, %lu,   %d, %d", Utils::millis(), lastKeepAlive, Utils::millis() - lastKeepAlive > 300, Ethernet::isConnected());
        if (((Utils::millis() - lastKeepAlive) > 300) && Ethernet::isConnected()){
            SendKeepAlive();
            lastKeepAlive = Utils::millis();
        }
    }

    void SendKeepAlive()
    {
        uint8_t device_id = 5;
        uint8_t buffer[] = {
            // proDJLink header
            0x51, 0x73, 0x70, 0x74, 0x31, 0x57, 0x6d, 0x4a, 0x4f, 0x4c,

            // keep alive packet type indicator
            PACKET_TYPE_KEEP_ALIVE, 0x00,

            // device name
            0, 0, 0, 0, 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

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

        uint32_t ip = htonl(Ethernet::getIp().toUint32());

        memcpy(buffer + 0x26, Ethernet::getMac().octets, 6);
        memcpy(buffer + 0x2c, &ip, 4);

        // https://djl-analysis.deepsymmetry.org/djl-analysis/startup.html#startup-3000
        // cdj3000 sends a different keep alive packet
        // I don't have these players here to test in depth, but when i detect a single cdj3000
        // keep alive packet, i change over to the cdj3000 keep alive packet structure.
        // (This structure seems not compatible with earlier devices like the cdj2000.)
        if (syncWithCdj3000){
            buffer[0x30]=0x02;
            buffer[0x35]=0x64;
        }

        auto broadcastIp = IPAddress::broadcast();
        keepAliveSocket2.send(
            &broadcastIp,
            PRO_DJ_LINK_PORT_KEEP_ALIVE,
            buffer,
            sizeof(buffer));

        // Log::info(TAG,"sent keepalive %d.%d.%d.%d", buffer[0x2c], buffer[0x2d],buffer[0x2e],buffer[0x2f]);
        // Log::info(TAG,"sent keepalive mac %x:%x:%x:%x:%x:%x", buffer[0x26], buffer[0x27],buffer[0x28],buffer[0x29],buffer[0x2a],buffer[0x2b]);

    }
};

ProDJLinkTempo *ProDJLinkTempo::instance = nullptr;