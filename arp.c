#include "arp.h"
#include "interface.h"   // send_frame()
#include <string.h>
#include <arpa/inet.h>

static struct arp_entry arp_table[ARP_TABLE_SIZE];
static uint32_t local_ip;
static uint8_t  local_mac[6];

void arp_init(uint32_t my_ip, uint8_t my_mac[6])
{
    local_ip = my_ip;
    memcpy(local_mac, my_mac, 6);
    memset(arp_table, 0, sizeof(arp_table));
}

static void arp_table_insert(uint32_t ip, uint8_t mac[6])
{
    for (int i = 0; i < ARP_TABLE_SIZE; i++) {
        if (arp_table[i].ip == 0 || arp_table[i].ip == ip) {
            arp_table[i].ip = ip;
            memcpy(arp_table[i].mac, mac, 6);
            return;
        }
    }
}

void arp_handle(const uint8_t *packet, size_t len)
{
    if (len < sizeof(struct arp_header))
        return;

    struct arp_header *arp = (struct arp_header *)packet;

    if (ntohs(arp->htype) != ARP_HTYPE_ETHERNET ||
        ntohs(arp->ptype) != ARP_PTYPE_IPV4)
        return;

    if (ntohs(arp->oper) == ARP_REQUEST) {

        if (arp->tpa != local_ip)
            return;

        uint8_t buffer[sizeof(struct arp_header)];
        struct arp_header *reply = (struct arp_header *)buffer;

        reply->htype = htons(ARP_HTYPE_ETHERNET);
        reply->ptype = htons(ARP_PTYPE_IPV4);
        reply->hlen  = 6;
        reply->plen  = 4;
        reply->oper  = htons(ARP_REPLY);

        memcpy(reply->sha, local_mac, 6);
        reply->spa = local_ip;

        memcpy(reply->tha, arp->sha, 6);
        reply->tpa = arp->spa;

        send_frame(
            reply->tha,
            0x0806,                 // EtherType ARP
            buffer,
            sizeof(buffer)
        );
    }

    else if (ntohs(arp->oper) == ARP_REPLY) {
        arp_table_insert(arp->spa, arp->sha);
    }
}
