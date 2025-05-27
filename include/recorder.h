#ifndef RECORDER_H
#define RECORDER_H

#include <stdint.h>
#include "esp_err.h"
#include "esp_http_server.h"
#include "config.h"          // liefert FFT_SIZE, VREF, IREF …

#ifdef __cplusplus
extern "C" {
#endif

/* Rohwert-Puffer (12-Bit-ADC-Samples) */
extern uint16_t buf_voltage[FFT_SIZE];
extern uint16_t buf_current[FFT_SIZE];

/* Initialisierung */
void       recorder_init(void);

/* Scope über WebSocket senden */
esp_err_t  send_scope_frame(httpd_req_t *req);

/* CSV-Download (voltage,current pro Zeile) */
esp_err_t  download_handler(httpd_req_t *req);

#ifdef __cplusplus
}
#endif

#endif /* RECORDER_H */
