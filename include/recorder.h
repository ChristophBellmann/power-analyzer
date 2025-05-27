#pragma once
#include <stdint.h>
#include "esp_err.h"
#include "esp_http_server.h"
#include "config.h"          /* liefert FFT_SIZE, VREF, IREF … */

#ifdef __cplusplus
extern "C" {
#endif

/* Rohwert-Puffer (12-Bit-ADC-Samples) ------------------------- */
extern uint16_t buf_voltage[FFT_SIZE];
extern uint16_t buf_current[FFT_SIZE];

/* API --------------------------------------------------------- */
void       recorder_init(void);            /* Puffer / Tasks anlegen       */
void       send_scope_frame(void);         /* JSON-Frame an WebSocket o.ä. */
esp_err_t  download_handler(httpd_req_t *req); /* CSV-Download via HTTP    */

#ifdef __cplusplus
}
#endif
