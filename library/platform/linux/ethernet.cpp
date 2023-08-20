#include "platform/includes/ethernet.hpp"
#include "platform/includes/ipAddress.hpp"
#include "platform/includes/macAddress.hpp"
#include "platform/includes/log.hpp"

#include <stdio.h>      
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
//#include <net/if_dl.h>

static const char *TAG = "ETH";
MacAddress mac = MacAddress{0,0,0,0,0,0};

void Ethernet::initialize()
{
    // nothing to do, assume ethernet is initialized by os
    Log::info(TAG, "Got IP: %s", Ethernet::getIp().toString().c_str());
}

IPAddress Ethernet::getIp()
{
    //it is likely that your machine has a lot of ip addresses.
    //we use some heuristics here to find the one that is most likely
    //the most interesting one. 
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;
    sockaddr_in *ip4;
    sockaddr_in6 *ip6;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if (strncmp(ifa->ifa_name, "en", 2))
        {
            //on macos the wifi and eth interface names begin with 'en'
            //so skip if we got another name
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
            // is a valid IP4 Address
            // tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            // char addressBuffer[INET_ADDRSTRLEN];
            // inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            // printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer); 
            ip4 = (struct sockaddr_in *) ifa->ifa_addr;
        } else if (ifa->ifa_addr->sa_family == AF_INET6) { // check it is IP6
            // is a valid IP6 Address
            // tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            // char addressBuffer[INET6_ADDRSTRLEN];
            // inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            // printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer); 
            ip6 = (struct sockaddr_in6 *)ifa->ifa_addr;
        } 
    //     else if (ifa->ifa_addr->sa_family == AF_LINK) {
    //         struct sockaddr_dl* sdl = (struct sockaddr_dl *)ifa->ifa_addr;
    //         //unsigned char mac[6];
    //         //todo this only works on mac, not on linux.
    //         //also, there is no guarantee that this will pick the correct mac address
    //         if (6 == sdl->sdl_alen) {
    //             memcpy(mac, LLADDR(sdl), sdl->sdl_alen);
    //             //Log::info(TAG,"mac  : %02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    //         }
    //   }
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);

    return IPAddress(ip4,ip6);
}

MacAddress Ethernet::getMac()
{
    getIp();
    //Log::info(TAG,"mac2  : %02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return mac;
}

bool Ethernet::isConnected()
{
    return true;
}

bool Ethernet::isConnecting()
{
    return false;
}