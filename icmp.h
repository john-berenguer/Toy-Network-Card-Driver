#ifndef ICMP_H
#define ICMP_H

#include <stdint.h>
#include <stddef.h>

struct ip_header;

void icmp_handle(const struct ip_header *ip,
                 const uint8_t *payload,
                 size_t len);

#endif
