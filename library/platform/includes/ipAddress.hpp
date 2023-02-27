#pragma once
#include <string>
#include <arpa/inet.h>
#include <netdb.h>

#include "platform/includes/log.hpp"

// forward declaration;
class Socket;
class Ethernet;

class IPAddress
{
private:
    sockaddr_in ip4;
    sockaddr_in6 ip6;
    
    bool hasip4 = false;
    bool hasip6 = false;

    IPAddress(sockaddr_in ip4, sockaddr_in6 ip6)
    {
        this->ip4 = ip4;
        this->ip6 = ip6;
        this->hasip4= true;
        this->hasip6= true;
    }

    IPAddress(sockaddr_in ip4)
    {
        this->ip4 = ip4;
        this->hasip4= true;
    }

    IPAddress(sockaddr_in6 ip6)
    {
        this->ip6 = ip6;
        this->hasip6= true;
    }

    // IPAddress()
    // {
    //     Log::info("IPADDR","private constructor error");
    // }

    IPAddress(sockaddr_in *ip4, sockaddr_in6 *ip6)
    {
        if (ip4 != NULL)
        {
            this->ip4 = *(ip4);
            this->hasip4= true;
        }
        if (ip6 != NULL)
        {
            this->ip6 = *(ip6);
            this->hasip6= true;
        }
    }

    // get the ipv4 address
    sockaddr_in *getSockaddrIn()
    {
        if(!hasip4)
            return NULL;
        return &ip4;
    }

    // get the ipv6 address
    sockaddr_in6 *getSockaddrIn6()
    {
        if(!hasip6)
            return NULL;
        return &ip6;
    }

    // static IPAddress fromSockAddr_in(sockaddr_in *sockaddr_arg)
    // {
    //     return IPAddress(*(sockaddr_arg));
    // }

    // static IPAddress fromSockAddr_in6(sockaddr_in6 *sockaddr_arg)
    // {
    //     return IPAddress(*(sockaddr_arg));
    // }

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

        sockaddr_in *ip4;
        sockaddr_in6 *ip6;

        struct addrinfo *res = addressInfo;
        while (res)
        {
            if (ip4==NULL && res->ai_family == AF_INET)
                ip4 = (struct sockaddr_in *)res->ai_addr;
            else if (ip6==NULL && res->ai_family == AF_INET6) 
                ip6 = (struct sockaddr_in6 *)res->ai_addr;
            res = res->ai_next;
        }

        freeaddrinfo(addressInfo);

        return IPAddress(ip4,ip6);
    }

    static IPAddress fromUint32(uint32_t ipv4)
    {
        struct sockaddr_in sockaddr;
        memset(&sockaddr, 0, sizeof(sockaddr));
        sockaddr.sin_family = AF_INET;
        sockaddr.sin_addr.s_addr = htonl(ipv4);
        sockaddr.sin_len = sizeof(in_addr);

        return IPAddress(sockaddr);
    }

    static IPAddress fromIPString(const char *ip4)
    {
        struct sockaddr_in sockaddr;
        memset(&sockaddr, 0, sizeof(sockaddr));
        sockaddr.sin_family = AF_INET;
        sockaddr.sin_len = sizeof(in_addr);
        inet_pton(AF_INET, ip4, &sockaddr.sin_addr);

        return IPAddress(sockaddr);
    }

    const std::string toString()
    {
        char addrstr[INET_ADDRSTRLEN]; // INET6_ADDRSTRLEN
        inet_ntop(AF_INET, &(getSockaddrIn()->sin_addr), addrstr, sizeof(addrstr));
        return std::string(addrstr);
    }

    friend class Socket;
    friend class Ethernet;
};
