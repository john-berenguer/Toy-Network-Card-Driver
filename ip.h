#ifndef IP_H
#define IP_H

#include <stdint.h>
#include <stddef.h>

#define IPPROTO_ICMP 1
#define IPPROTO_TCP  6

struct ip_header {
    uint8_t  ver_ihl;      // version (4 bits) + IHL (4 bits)
    uint8_t  tos;
    uint16_t total_length;
    uint16_t id;
    uint16_t flags_frag;
    uint8_t  ttl;
    uint8_t  protocol;
    uint16_t checksum;
    uint32_t src_ip;
    uint32_t dst_ip;
} __attribute__((packed));

void ip_init(uint32_t my_ip);
void ip_handle(const uint8_t *packet, size_t len);

#endif
