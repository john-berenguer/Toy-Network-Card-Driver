#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "interface.h"
#include "arp.h"
#include "ip.h"

/* Ethernet RX callback */
void received_packet(const void *data, unsigned int length)
{
    if (length < 14)
        return;

    const uint8_t *frame = (const uint8_t *)data;

    uint16_t ethertype = ntohs(*(uint16_t *)(frame + 12));

    switch (ethertype) {

    case 0x0806: // ARP
        arp_handle(frame + 14, length - 14);
        break;

    case 0x0800: // IPv4
        ip_handle(frame + 14, length - 14);
        break;

    default:
        break;
    }
}

int main(int argc, char* argv[])
{
    nic_device_t nic;
    nic_driver_t *drv = nic_get_driver();

    if (drv->init(&nic) != STATUS_OK) {
        printf("Failed to initialize NIC\n");
        return -1;
    }

    /* IP fija (CAMBIA si hace falta) */
    uint32_t my_ip = inet_addr("192.168.56.100");

    arp_init(my_ip, nic.mac_address);
    ip_init(my_ip);

    if (drv->ioctl(&nic, NIC_IOCTL_ADD_RX_CALLBACK,
                   (void *)&received_packet) != STATUS_OK) {
        printf("Failed to add RX callback\n");
        drv->shutdown(&nic);
        return -1;
    }

    printf("NIC up. Listening for packets...\n");

    /* Mantener el programa vivo */
    while (1) {
        sleep(1);
    }

    drv->shutdown(&nic);
    return 0;
}

