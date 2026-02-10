#ifndef HTTP_H
#define HTTP_H

#include <stdint.h>
#include <stddef.h>

void http_handle(const uint8_t *data, size_t len);

#endif
