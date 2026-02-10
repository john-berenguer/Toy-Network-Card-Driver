#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdint.h>
#include <stddef.h>
#include "hal.h"

void interface_init(void);

/* Ya lo usas en IP */
struct ip_header;
void send_ip_packet(const struct ip_header *ip,
                    const uint8_t *payload,
                    size_t payload_len);

#endif
