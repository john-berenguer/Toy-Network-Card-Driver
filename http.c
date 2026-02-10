#include "http.h"
#include "tcp.h"

#include <string.h>

static const char http_response[] =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/plain\r\n"
    "Content-Length: 11\r\n"
    "\r\n"
    "Hola mundo";

void http_handle(const uint8_t *data, size_t len)
{
    /* No parseamos nada de verdad */
    /* Si llega cualquier cosa → respondemos */

    tcp_send_data(
        (const uint8_t *)http_response,
        sizeof(http_response) - 1
    );
}
