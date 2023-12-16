#pragma once
#include "abstractTempo.h"
#include "socket.hpp"
#include "log.hpp"
#include "utils.hpp"

#define PRO_DJ_LINK_PORT_KEEP_ALIVE 50000
#define PRO_DJ_LINK_PORT_SYNC 50001
#define PRO_DJ_LINK_PORT_STATUS 50002

//ProDJLinkTempo will connect to Pioneer cdj/xdj's and sync to their beats.
//only works if the tracks are analyzed with recordbox.
class ProDJLinkTempo : public AbstractTempo
{
private:
    static ProDJLinkTempo * instance;
public:
    ProDJLinkTempo()
    {
        sourceName = "ProDJLink";
    }

    static ProDJLinkTempo *getInstance(){
        if (!instance)
            instance = new ProDJLinkTempo();
        return instance;
    }

private:
    Socket syncSocket = Socket(PRO_DJ_LINK_PORT_SYNC);
    Socket statusSocket = Socket(PRO_DJ_LINK_PORT_STATUS);
    Socket keepAliveSocket = Socket(PRO_DJ_LINK_PORT_KEEP_ALIVE);
    unsigned long lastKeepAlive = 0; //when the last keep alive was sent to the other players
    int master = -1;                  //the cd player that is currently tempo master
    uint8_t buffer[1500]; //TODO should only need 0x11c bytes, test

    //this task handles all communication with the cd players
    void TempoTask() 
    {
        //https://djl-analysis.deepsymmetry.org/djl-analysis/beats.html#_footnoteref_1

        //TODO make this a while loop
        int len = syncSocket.receive(buffer, sizeof(buffer));
        if (len > 0)
        {
            int device = buffer[0x5f];
            if (master==-1) master = device;
            if (len == 0x60 && buffer[0x0a] == 0x28 && device == master)
            {
                beat();

                //adjust the beat counter if it is not in sync with the beat counter of the cdj anymore.
                int beatmod4cdj = buffer[0x5c] - 1;        //beat number 0-3 from the cdj
                int beatmod4local = beatNumber % 4; //beat number 0-3 from this Tempo class
                int diff = beatmod4cdj - beatmod4local;
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
            validSignal=true;
            Log::info(TAG,"len=%d, id=%d, beat=%d, device=%d, master=%d\r\n", len, buffer[0x21], buffer[0x5c], buffer[0x5f], master);
        }

        len = statusSocket.receive(buffer,sizeof(buffer));
        if (len > 0)
        {
            Log::info(TAG,"status %x, %x\r\n", len, buffer[0x0a]);
            if (len == 0x11c && buffer[0x0a] == 0x0a)
            {
                int device = buffer[0x21];
                int ismaster = buffer[0x9e] > 0;
                if (ismaster > 0 &&master != device)
                {
                    master = device;
                    Log::info(TAG,"master handoff to %d", device);
                }
            }
        }

        len = keepAliveSocket.receive(buffer,sizeof(buffer));
        if (len > 0)
        {
            Log::info(TAG,"got keep alive %x, %x\r\n", len, buffer[0x0a]);
        }

        if (Utils::millis() - lastKeepAlive > 300 && Ethernet::isConnected())
            SendKeepAlive();
    }

    void SendKeepAlive()
    {
        uint8_t device_id = 5;
        uint8_t buffer[] = {
            0x51,
            0x73,
            0x70,
            0x74,
            0x31,
            0x57,
            0x6d,
            0x4a,
            0x4f,
            0x4c,
            0x06,
            0x00,

            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

            1, 2, 0, 0x36, device_id, 01,

            0, 0, 0, 0, 0, 0, //mac
            0, 0, 0, 0,       //ip

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

        // this2->statusSocket.beginPacket(IPAddress(255, 255, 255, 255), PRO_DJ_LINK_PORT_KEEP_ALIVE);
        // this2->statusSocket.write(buffer, sizeof(buffer));
        // this2->statusSocket.endPacket();

        lastKeepAlive = Utils::millis();

        // Log::info(TAG,"send keepalive");
    }
};

ProDJLinkTempo *ProDJLinkTempo::instance = nullptr;