#ifndef WIFI_H
#define WIFI_H

#include "esp_err.h"
#include "esp_http_server.h"

#ifdef __cplusplus
extern "C" {
#endif

void wifi_init_sta(void);
httpd_handle_t start_webserver(void);

// If you previously had wifi_broadcast_msg(...), remove or comment it 
// because it relies on httpd_ws_create_req() which is unavailable in your IDF.
// esp_err_t wifi_broadcast_msg(const char *payload);

#ifdef __cplusplus
}
#endif

#endif // WIFI_H
