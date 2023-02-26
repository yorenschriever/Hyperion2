#pragma once
#include <string>

#include <arpa/inet.h>
#include <netdb.h>

// forward declaration;
class Socket;

class IPAddress
{
private:
    addrinfo *addressInfo;
    bool error = false;

    IPAddress(addrinfo *addressInfo, bool error = false)
    {
        this->addressInfo = addressInfo;
        this->error = error;
    }

    // get the ipv4 address
    sockaddr_in *getSockaddrIn()
    {
        struct addrinfo *res = addressInfo;
        while (res)
        {
            if (res->ai_family == AF_INET)
            {
                return (struct sockaddr_in *)res->ai_addr;
            }
            res = res->ai_next;
        }

        return NULL;
    }

    // get the ipv6 address
    sockaddr_in6 *getSockaddrIn6()
    {
        struct addrinfo *res = addressInfo;
        while (res)
        {
            if (res->ai_family == AF_INET6)
            {
                return (struct sockaddr_in6 *)res->ai_addr;
            }
            res = res->ai_next;
        }

        return NULL;
    }

public:
    static IPAddress fromHostName(const char *hostname)
    {
        struct addrinfo hints, *addressInfo;
        int errcode;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = PF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags |= AI_CANONNAME;

        errcode = getaddrinfo(hostname, NULL, &hints, &addressInfo);
        return IPAddress(addressInfo, errcode != 0);
    }

    static IPAddress fromSockAddr_in(sockaddr_in *sockaddr_arg)
    {
        // allocate new addrinfo and sockaddr. The destructor of IPAddress will free this
        addrinfo *addressInfo = new addrinfo;
        struct sockaddr_in *sockaddr_clone = new struct sockaddr_in;
        memcpy(sockaddr_clone, sockaddr_arg, sizeof(sockaddr_in));

        memset(addressInfo, 0, sizeof(addrinfo));
        addressInfo->ai_family = AF_INET; // AF_UNSPEC; // use AF_INET6 to force IPv6
        addressInfo->ai_addr = (sockaddr *)sockaddr_clone;

        return IPAddress(addressInfo);
    }

    static IPAddress fromSockAddr_in6(sockaddr_in6 *sockaddr_arg)
    {
        // allocate new addrinfo and sockaddr. The destructor of IPAddress will free this
        addrinfo *addressInfo = new addrinfo;
        struct sockaddr_in6 *sockaddr_clone = new struct sockaddr_in6;
        memcpy(sockaddr_clone, sockaddr_arg, sizeof(sockaddr_in6));

        memset(addressInfo, 0, sizeof(addrinfo));
        addressInfo->ai_family = AF_INET6; // AF_UNSPEC; // use AF_INET6 to force IPv6
        addressInfo->ai_addr = (sockaddr *)sockaddr_clone;
        addressInfo->ai_next = NULL;

        return IPAddress(addressInfo);
    }

    static IPAddress fromUint32(uint32_t ipv4)
    {
        struct sockaddr_in sockaddr;
        memset(&sockaddr, 0, sizeof(sockaddr));
        sockaddr.sin_family = AF_INET;
        sockaddr.sin_addr.s_addr = htonl(ipv4);
        sockaddr.sin_len = sizeof(in_addr);

        return IPAddress::fromSockAddr_in(&sockaddr);
    }

    static IPAddress fromIPString(const char *ip4)
    {
        struct sockaddr_in sockaddr;
        memset(&sockaddr, 0, sizeof(sockaddr));
        sockaddr.sin_family = AF_INET;
        sockaddr.sin_len = sizeof(in_addr);
        inet_pton(AF_INET, ip4, &sockaddr.sin_addr);

        return IPAddress::fromSockAddr_in(&sockaddr);
    }

    const std::string toString()
    {
        char addrstr[INET_ADDRSTRLEN]; // INET6_ADDRSTRLEN
        inet_ntop(AF_INET, &(getSockaddrIn()->sin_addr), addrstr, sizeof(addrstr));
        return std::string(addrstr);
    }

    ~IPAddress()
    {
        freeaddrinfo(addressInfo);
    }

    friend class Socket;
};
