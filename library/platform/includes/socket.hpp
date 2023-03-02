#pragma once
#include "ipAddress.hpp"
#include "ethernet.hpp"
#include "log.hpp"
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>

static const char *TAG = "SOCK";

class Socket
{

private:
    int addr_family = AF_INET;
    int sock = 0;

public:
    Socket(uint16_t port = 0, int addr_family = AF_INET)
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

        set_recv_timeout(0);

        if (port != 0)
        {
            err = ::bind(sock, (struct sockaddr *)&dest_addr, dest_addr_size);
            if (err != 0)
            {
                Log::error(TAG, "unable to bind to port %d. error code %d", port, sock);
            }
            else
            {
                Log::info(TAG, "socket bound to port %d", port);
            }
        }
    }

    void send(IPAddress *destination, uint16_t port, uint8_t *payload, unsigned int len)
    {
        if (destination == NULL)
            return;

        if (!Ethernet::isConnected())
        {
            //lwip stack initializes on eth connect event (or wifi), before that we will get an error
            Log::error(TAG, "cannot send. eth not connected");
            return;
        }

        if (sock <= 0)
        {
            Log::error(TAG, "cannot send. socket not open");
            return;
        }

        int err;
        if (addr_family == AF_INET6)
        {
            auto dest_addr = *(destination->getSockaddrIn6());
            dest_addr.sin6_port = htons(port);
            err = ::sendto(sock, (void *)payload, len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        }
        else
        {
            auto dest_addr = *(destination->getSockaddrIn());
            dest_addr.sin_port = htons(port);
            err = ::sendto(sock, (void *)payload, len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        }

        if (err < 0)
        {
            Log::error(TAG, "error occurred during sending. error code %d", err);
        }
    }

    int recv(void *rx_buffer, unsigned int buffer_length)
    {
        if (sock <= 0)
        {
            Log::error(TAG, "cannot recv. socket not open");
            return 0;
        }

        struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
        socklen_t socklen = sizeof(source_addr);
        int len = recvfrom(sock, rx_buffer, buffer_length, 0, (struct sockaddr *)&source_addr, &socklen);

        if (len < 0 && errno != EWOULDBLOCK)
        {
            Log::error(TAG, "error during recvfrom: errno: %d", errno);
        }

        return len;
    }

    int set_recv_timeout(int ms)
    {
        if (ms == 0){
            const int flags = fcntl(sock, F_GETFL, 0);
            if (flags < 0) {
                Log::error(TAG, "unable to set non blocking node"); 
                return -1;
            }
            if (fcntl(sock, F_SETFL, flags|O_NONBLOCK) < 0) {
                Log::error(TAG, "unable to set non blocking mode");
                return -1;
            }
        } 

        struct timeval timeout;
        timeout.tv_sec = ms/1000;
        timeout.tv_usec = (ms%1000)*1000;
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
