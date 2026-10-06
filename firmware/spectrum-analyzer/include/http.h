#ifndef HTTP_H
#define HTTP_H

#include "esp_http_server.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Startet den HTTP-Server und registriert alle URI-Handler.
 *
 * @return httpd_handle_t Handle des gestarteten HTTP-Servers.
 */
httpd_handle_t start_webserver(void);

#ifdef __cplusplus
}
#endif

#endif // HTTP_H
