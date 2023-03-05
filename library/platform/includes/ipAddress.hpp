#pragma once
#include <arpa/inet.h>
#include <netdb.h>
#include <string>

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
        this->hasip4 = true;
        this->hasip6 = true;
    }

    IPAddress(sockaddr_in ip4)
    {
        this->ip4 = ip4;
        this->hasip4 = true;
    }

    IPAddress(sockaddr_in6 ip6)
    {
        this->ip6 = ip6;
        this->hasip6 = true;
    }

    IPAddress(sockaddr_in *ip4, sockaddr_in6 *ip6)
    {
        if (ip4 != NULL)
        {
            this->ip4 = *(ip4);
            this->hasip4 = true;
        }
        if (ip6 != NULL)
        {
            this->ip6 = *(ip6);
            this->hasip6 = true;
        }
    }

public:
    static IPAddress fromHostname(const char *hostname)
    {
        struct addrinfo hints, *addressInfo;
        int errcode;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = PF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags |= AI_CANONNAME;

        errcode = getaddrinfo(hostname, NULL, &hints, &addressInfo);
        if (errcode != 0)
        {
            Log::error("IPADDRESS", "error resolving hostname %s", hostname);
            return IPAddress(NULL, NULL);
        }

        IPAddress result = IPAddress(NULL, NULL);
        struct addrinfo *res = addressInfo;

        while (res)
        {
            if (!result.hasip4 && res->ai_family == AF_INET)
            {
                result.ip4 = *((struct sockaddr_in *)res->ai_addr);
                result.hasip4 = true;
            }
            else if (!result.hasip6 && res->ai_family == AF_INET6)
            {
                result.ip6 = *((struct sockaddr_in6 *)res->ai_addr);
                result.hasip6 = true;
            }
            res = res->ai_next;
        }
        freeaddrinfo(addressInfo);

        return result;
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
        if (!hasip4)
            return "[none]";
        char addrstr[INET_ADDRSTRLEN]; // INET6_ADDRSTRLEN
        inet_ntop(AF_INET, &(ip4.sin_addr), addrstr, sizeof(addrstr));
        return std::string(addrstr);
    }

    bool isValid() { return hasip4 || hasip6; }

    friend class Socket;
    friend class Ethernet;
};
