#include <stdint.h>

struct hyp_ip4_addr_t
{
    uint32_t addr;
};

struct hyp_ip6_addr_t
{
    uint32_t addr[4];
    uint8_t zone;
};

//typedef 
struct IPAddress {
    // union {
    //     hyp_ip6_addr_t ip6; /*!< IPv6 address type */
    //     hyp_ip4_addr_t ip4; /*!< IPv4 address type */
    // } u_addr;               /*!< IP address union */
    uint8_t type;           /*!< ipaddress type */
} ;
