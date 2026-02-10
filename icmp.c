#include "icmp.h"
#include "ip.h"
#include "interface.h"

#include <string.h>
#include <arpa/inet.h>

#define ICMP_ECHO_REQUEST 8
#define ICMP_ECHO_REPLY   0

struct icmp_header {
    uint8_t  type;
    uint8_t  code;
    uint16_t checksum;
    uint16_t id;
    uint16_t sequence;
} __attribute__((packed));

static uint16_t icmp_checksum(const void *data, size_t len)
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

void icmp_handle(const struct ip_header *ip,
                 const uint8_t *payload,
                 size_t len)
{
    if (len < sizeof(struct icmp_header))
        return;

    struct icmp_header *icmp = (struct icmp_header *)payload;

    if (icmp->type != ICMP_ECHO_REQUEST)
        return;

    uint8_t buffer[1500];

    /* Construir ICMP reply */
    struct icmp_header *reply =
        (struct icmp_header *)buffer;

    memcpy(buffer, payload, len);

    reply->type = ICMP_ECHO_REPLY;
    reply->code = 0;
    reply->checksum = 0;
    reply->checksum = icmp_checksum(buffer, len);

    /* Construir IP */
    struct ip_header *ip_reply =
        (struct ip_header *)(buffer - sizeof(struct ip_header));

    ip_reply->ver_ihl = 0x45;
    ip_reply->tos = 0;
    ip_reply->total_length =
        htons(sizeof(struct ip_header) + len);
    ip_reply->id = 0;
    ip_reply->flags_frag = 0;
    ip_reply->ttl = 64;
    ip_reply->protocol = IPPROTO_ICMP;
    ip_reply->src_ip = ip->dst_ip;
    ip_reply->dst_ip = ip->src_ip;
    ip_reply->checksum = 0;
    ip_reply->checksum =
        ip_checksum(ip_reply, sizeof(struct ip_header));

    /* Enviar */
    send_ip_packet(
        ip_reply,
        buffer,
        len
    );
}
