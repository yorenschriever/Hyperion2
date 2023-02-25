#pragma once
#include "ipAddress.hpp"
#include "log.hpp"
#include <netdb.h>
#include <unistd.h>

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
        int dest_addr_size=0;
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

        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        err = ::setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);
        if (err != 0)
        {
            Log::error(TAG, "unable to set recv timeout");
        }        

        if (port != 0)
        {
            err = ::bind(sock, (struct sockaddr *)&dest_addr, dest_addr_size);
            if (err != 0)
            {
                Log::error(TAG, "unable to bind to port %d. error code %d", port, sock);
            } else {
                Log::info(TAG, "socket bound to port %d", port);
            }
        }
    }

    void send(IPAddress destination, uint16_t port, uint8_t *payload, unsigned int len)
    {
        if (sock <= 0)
        {
            Log::error(TAG, "cannot send. socket not open");
            return;
        }

        // struct sockaddr dest_addr;
        int err;
        if (addr_family == AF_INET6)
        {
            auto dest_addr = *(destination.getSockaddrIn6());
            dest_addr.sin6_port = htons(port); 
            err = ::sendto(sock, (void *)payload, len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        }
        else
        {
            auto dest_addr = *(destination.getSockaddrIn());
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

        if (len < 0) {
            Log::error(TAG, "error during recvfrom: errno: %d",len);
        }

        return len;
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
