#pragma once
#include "ethernet.hpp"
#include "ipAddress.hpp"
#include "log.hpp"
#include <fcntl.h>
#include <netdb.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

static const char *TAG = "SOCK";

class Socket
{

private:
    int addr_family = AF_INET;
    int sock = 0;

public:
    Socket(uint16_t port = 0, int addr_family = AF_INET, int receive_timeout=0, bool broadcast=false)
    {
        int err;
        this->addr_family = addr_family;
        struct sockaddr_in6 dest_addr;
        int dest_addr_size = 0;
        ::bzero(&dest_addr, sizeof(dest_addr));

        int ip_protocol = 0;
        if (addr_family == AF_INET)
        {
            struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
            dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
            dest_addr_ip4->sin_family = AF_INET;
            dest_addr_ip4->sin_port = htons(port);
            ip_protocol = IPPROTO_IP;
            dest_addr_size = sizeof(sockaddr_in);
        }
        else if (addr_family == AF_INET6)
        {
            ::bzero(&dest_addr.sin6_addr, sizeof(dest_addr.sin6_addr));
            dest_addr.sin6_family = AF_INET6;
            dest_addr.sin6_port = htons(port);
            ip_protocol = IPPROTO_IPV6;
            dest_addr_size = sizeof(sockaddr_in6);
        }
        else
        {
            Log::error(TAG, "invalid addr_family %d", addr_family);
            sock = 0;
        }

        sock = ::socket(addr_family, SOCK_DGRAM, ip_protocol);
        if (sock < 0)
        {
            Log::error(TAG, "unable to open socket. error code %d", sock);
            return;
        }

        set_recv_timeout(receive_timeout);

        if (port != 0)
        {
            err = ::bind(sock, (struct sockaddr *)&dest_addr, dest_addr_size);
            if (err != 0)
            {
                Log::error(TAG, "unable to bind to port %d. error code %d", port, sock);
            }
        }

        if (broadcast){
            int broadcastEnable=1;
            setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
        }
    }

    int send(IPAddress *destination, uint16_t port, uint8_t *payload, unsigned int len)
    {
        if (destination == NULL)
            return 0;

        if (!Ethernet::isConnected())
        {
            // lwip stack initializes on eth connect event (or wifi), before that we will get an error
            Log::error(TAG, "cannot send. eth not connected");
            return -1;
        }

        if (sock <= 0)
        {
            Log::error(TAG, "cannot send. socket not open");
            return -1;
        }

        int err;
        if (addr_family == AF_INET6)
        {
            if (!destination->hasip6)
            {
                Log::error(TAG, "cannot use ipv6 socket for this ipaddress");
                return -1;
            }
            auto dest_addr = *(&destination->ip6);
            dest_addr.sin6_port = htons(port);
            err = ::sendto(sock, (void *)payload, len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        }
        else
        {
            if (!destination->hasip4)
            {
                Log::error(TAG, "cannot use ipv4 socket for this ipaddress");
                return -1;
            }
            //Log::info(TAG,"sending to %s:%d", destination->toString().c_str(), port);
            auto dest_addr = *(&destination->ip4);
            dest_addr.sin_port = htons(port);
            err = ::sendto(sock, (void *)payload, len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        }

        if (err < 0)
        {
            if (errno == 64){
                //Client device is probably not connected. 
                //Log::error(TAG, "no route to host");
                return err;
            }
            Log::error(TAG, "error occurred during sending. error code %d, %d", err, errno);
            return err;
        }
        else
        {
            // Log::info(TAG, "send %d bytes to %d", len, port);
        }
        return 0;
    }

    int receive(void *rx_buffer, unsigned int buffer_length)
    {
        if (sock <= 0)
        {
            Log::error(TAG, "cannot recv. socket not open");
            return 0;
        }

        int len = recv(sock, rx_buffer, buffer_length, MSG_DONTWAIT);

        if (len < 0 && errno != EWOULDBLOCK)
        {
            Log::error(TAG, "error during recvfrom: errno: %d", errno);
            return 0;
        }

        return len;
    }

    int receive_wait(void *rx_buffer, unsigned int buffer_length)
    {
        if (sock <= 0)
        {
            Log::error(TAG, "cannot recv. socket not open");
            return 0;
        }

        int len = recv(sock, rx_buffer, buffer_length, MSG_WAITALL);

        if (len < 0 && errno != EAGAIN)
        {
            Log::error(TAG, "error during recvfrom: errno: %d", errno);
        }

        return len;
    }

    int set_recv_timeout(int ms)
    {
        if (ms == 0)
        {
            const int flags = fcntl(sock, F_GETFL, 0);
            if (flags == -1)
            {
                Log::error(TAG, "unable to set non blocking node");
                return -1;
            }
            if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) != 0)
            {
                Log::error(TAG, "unable to set non blocking mode");
                return -1;
            }
            return 0;
        }

        struct timeval timeout;
        timeout.tv_sec = ms / 1000;
        timeout.tv_usec = (ms % 1000) * 1000;
        int err = ::setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);
        if (err != 0)
        {
            Log::error(TAG, "unable to set recv timeout");
        }
        return err;
    }

    ~Socket()
    {
        if (sock != -1)
        {
            ::shutdown(sock, 0);
            ::close(sock);
        }
    }
};
