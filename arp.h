
#ifndef ARP_H
#define ARP_H

#include <stdint.h>
#include <stddef.h>

#define ARP_TABLE_SIZE 16

#define ARP_HTYPE_ETHERNET 1
#define ARP_PTYPE_IPV4    0x0800

#define ARP_REQUEST 1
#define ARP_REPLY   2

struct arp_header {
    uint16_t htype;
    uint16_t ptype;
    uint8_t  hlen;
    uint8_t  plen;
    uint16_t oper;
    uint8_t  sha[6];
    uint32_t spa;
    uint8_t  tha[6];
    uint32_t tpa;
} __attribute__((packed));

struct arp_entry {
    uint32_t ip;
    uint8_t  mac[6];
};

void arp_init(uint32_t my_ip, uint8_t my_mac[6]);
void arp_handle(const uint8_t *packet, size_t len);

#endif
