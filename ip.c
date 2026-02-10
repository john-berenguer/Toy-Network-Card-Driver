#include "ip.h"
#include "icmp.h"
#include "tcp.h"
#include "interface.h"

#include <string.h>
#include <arpa/inet.h>

extern uint32_t local_ip;

/* ================================================= */
/* IP checksum                                       */
/* ================================================= */
uint16_t ip_checksum(const void *buf, size_t len)
{
    uint32_t sum = 0;
    const uint16_t *data = buf;

    while (len > 1) {
        sum += *data++;
        len -= 2;
    }

    if (len)
        sum += *(uint8_t *)data;

    while (sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);

    return ~sum;
}

/* ================================================= */
/* IP RX                                             */
/* ================================================= */
void ip_handle(const uint8_t *packet, size_t len)
{
    if (len < sizeof(struct ip_header))
        return;

    const struct ip_header *ip =
        (const struct ip_header *)packet;

    size_t ihl = (ip->ver_ihl & 0x0F) * 4;

    if (len < ihl)
        return;

    const uint8_t *payload = packet + ihl;
    size_t payload_len = len - ihl;

    if (ip->dst_ip != local_ip)
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

/* ================================================= */
/* IP TX                                             */
/* ================================================= */
void ip_send(uint32_t dst_ip,
             uint8_t protocol,
             const uint8_t *payload,
             size_t payload_len)
{
    uint8_t buffer[1500];

    struct ip_header *ip =
        (struct ip_header *)buffer;

    ip->ver_ihl = 0x45;
    ip->tos = 0;
    ip->total_length =
        htons(sizeof(struct ip_header) + payload_len);
    ip->id = 0;
    ip->flags_frag = 0;
    ip->ttl = 64;
    ip->protocol = protocol;
    ip->src_ip = local_ip;
    ip->dst_ip = dst_ip;
    ip->checksum = 0;
    ip->checksum =
        ip_checksum(ip, sizeof(struct ip_header));

    memcpy(buffer + sizeof(struct ip_header),
           payload,
           payload_len);

    send_ip_packet(ip,
                   payload,
                   payload_len);
}
