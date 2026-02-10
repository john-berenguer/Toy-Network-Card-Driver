#ifndef TCP_H
#define TCP_H

#include <stdint.h>
#include <stddef.h>

struct ip_header;

void tcp_handle(const struct ip_header *ip,
                const uint8_t *payload,
                size_t len);

#endif
