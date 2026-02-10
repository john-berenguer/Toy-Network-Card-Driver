#include "ip.h"
#include "icmp.h"
#include "tcp.h"

#include <string.h>
#include <arpa/inet.h>

static uint32_t local_ip;

/* Checksum IPv4 estándar */
static uint16_t ip_checksum(const void *data, size_t len)
{
    const uint16_t *buf = data;
    uint32_t sum = 0;

    while (len > 1) {
        sum += *buf++;
        len -= 2;
    }

    if (len)
        sum += *(uint8_t *)buf;

    while (sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);

    return (uint16_t)(~sum);
}

void ip_init(uint32_t my_ip)
{
    local_ip = my_ip;
}

void ip_handle(const uint8_t *packet, size_t len)
{
    if (len < sizeof(struct ip_header))
        return;

    struct ip_header *ip = (struct ip_header *)packet;

    uint8_t version = ip->ver_ihl >> 4;
    uint8_t ihl = (ip->ver_ihl & 0x0F) * 4;

    if (version != 4)
        return;

    if (ihl < sizeof(struct ip_header))
        return;

    if (len < ihl)
        return;

    if (ip->dst_ip != local_ip)
        return;

    /* verificar checksum */
    uint16_t received = ip->checksum;
    ip->checksum = 0;

    if (ip_checksum(ip, ihl) != received)
        return;

    ip->checksum = received;

    const uint8_t *payload = packet + ihl;
    size_t payload_len = ntohs(ip->total_length) - ihl;

    if (payload_len > len - ihl)
        return;

    switch (ip->protocol) {

    case IPPROTO_ICMP:
        icmp_handle(ip, payload, payload_len);
        break;

    case IPPROTO_TCP:
        tcp_handle(ip, payload, payload_len);
        break;

    default:
        break;
    }
}
