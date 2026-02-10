#include "tcp.h"
#include "ip.h"

#include <string.h>
#include <arpa/inet.h>

/* Flags TCP */
#define TCP_FLAG_FIN 0x01
#define TCP_FLAG_SYN 0x02
#define TCP_FLAG_ACK 0x10

/* Estados mínimos */
#define TCP_STATE_LISTEN       0
#define TCP_STATE_SYN_RECEIVED 1
#define TCP_STATE_ESTABLISHED  2
#define TCP_STATE_FIN          3

struct tcp_header {
    uint16_t src_port;
    uint16_t dst_port;
    uint32_t seq;
    uint32_t ack;
    uint8_t  offset_reserved;
    uint8_t  flags;
    uint16_t window;
    uint16_t checksum;
    uint16_t urgent;
} __attribute__((packed));

/* Estado global (UNA conexión) */
static int tcp_state = TCP_STATE_LISTEN;
static uint32_t local_seq;
static uint32_t remote_seq;
static uint16_t remote_port;
static uint32_t remote_ip;

/* ============================= */
/* Checksum TCP (pseudo-header)  */
/* ============================= */
static uint16_t tcp_checksum(uint32_t src_ip,
                             uint32_t dst_ip,
                             const struct tcp_header *tcp,
                             const uint8_t *payload,
                             size_t payload_len)
{
    uint32_t sum = 0;

    sum += (src_ip >> 16) & 0xFFFF;
    sum += src_ip & 0xFFFF;
    sum += (dst_ip >> 16) & 0xFFFF;
    sum += dst_ip & 0xFFFF;
    sum += htons(IPPROTO_TCP);
    sum += htons(sizeof(struct tcp_header) + payload_len);

    const uint16_t *buf = (const uint16_t *)tcp;
    size_t len = sizeof(struct tcp_header);

    while (len > 1) {
        sum += *buf++;
        len -= 2;
    }

    buf = (const uint16_t *)payload;
    len = payload_len;

    while (len > 1) {
        sum += *buf++;
        len -= 2;
    }

    if (len)
        sum += *(const uint8_t *)buf;

    while (sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);

    return (uint16_t)(~sum);
}

/* ============================= */
/* Envío de segmento TCP         */
/* ============================= */
static void tcp_send(uint8_t flags,
                     const uint8_t *payload,
                     size_t payload_len)
{
    uint8_t buffer[1500];

    struct tcp_header *tcp =
        (struct tcp_header *)buffer;

    tcp->src_port = htons(80);
    tcp->dst_port = htons(remote_port);
    tcp->seq = htonl(local_seq);
    tcp->ack = htonl(remote_seq);
    tcp->offset_reserved = (5 << 4);
    tcp->flags = flags;
    tcp->window = htons(1024);
    tcp->urgent = 0;
    tcp->checksum = 0;

    if (payload_len)
        memcpy(buffer + sizeof(struct tcp_header),
               payload,
               payload_len);

    tcp->checksum =
        tcp_checksum(local_ip,
                     remote_ip,
                     tcp,
                     payload,
                     payload_len);

    ip_send(remote_ip,
            IPPROTO_TCP,
            buffer,
            sizeof(struct tcp_header) + payload_len);
}

/* ============================= */
/* Recepción TCP                 */
/* ============================= */
void tcp_handle(const struct ip_header *ip,
                const uint8_t *payload,
                size_t len)
{
    if (len < sizeof(struct tcp_header))
        return;

    const struct tcp_header *tcp =
        (const struct tcp_header *)payload;

    if (ntohs(tcp->dst_port) != 80)
        return;

    uint8_t flags = tcp->flags;
    uint32_t seq = ntohl(tcp->seq);

    switch (tcp_state) {

    case TCP_STATE_LISTEN:
        if (flags & TCP_FLAG_SYN) {
            remote_ip = ip->src_ip;
            remote_port = ntohs(tcp->src_port);
            remote_seq = seq + 1;
            local_seq = 1;

            tcp_send(TCP_FLAG_SYN | TCP_FLAG_ACK,
                     NULL, 0);

            tcp_state = TCP_STATE_SYN_RECEIVED;
        }
        break;

    case TCP_STATE_SYN_RECEIVED:
        if (flags & TCP_FLAG_ACK) {
            tcp_state = TCP_STATE_ESTABLISHED;
        }
        break;

    case TCP_STATE_ESTABLISHED:
        if (flags & TCP_FLAG_FIN) {
            remote_seq = seq + 1;

            tcp_send(TCP_FLAG_ACK | TCP_FLAG_FIN,
                     NULL, 0);

            tcp_state = TCP_STATE_FIN;
        }
        break;

    default:
        break;
    }
}
