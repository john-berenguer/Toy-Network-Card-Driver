#include "interface.h"
#include "arp.h"
#include "ip.h"

#include <string.h>
#include <arpa/inet.h>

/* Ethernet */
#define ETH_TYPE_IP   0x0800
#define ETH_TYPE_ARP  0x0806
#define ETH_ADDR_LEN  6

struct eth_header {
    uint8_t  dst[ETH_ADDR_LEN];
    uint8_t  src[ETH_ADDR_LEN];
    uint16_t type;
} __attribute__((packed));

extern nic_device_t *nic_device;

/* ================================================= */
/* RX callback                                       */
/* ================================================= */
static void interface_rx_callback(const void *data,
                                  unsigned int length)
{
    if (length < sizeof(struct eth_header))
        return;

    const struct eth_header *eth =
        (const struct eth_header *)data;

    uint16_t type = ntohs(eth->type);

    const uint8_t *payload =
        (const uint8_t *)data + sizeof(struct eth_header);

    size_t payload_len =
        length - sizeof(struct eth_header);

    switch (type) {

    case ETH_TYPE_ARP:
        arp_handle(payload, payload_len);
        break;

    case ETH_TYPE_IP:
        ip_handle(payload, payload_len);
        break;

    default:
        break;
    }
}

/* ================================================= */
/* Inicialización                                    */
/* ================================================= */
void interface_init(void)
{
    nic_ioctl(nic_device,
              NIC_IOCTL_ADD_RX_CALLBACK,
              interface_rx_callback);
}

