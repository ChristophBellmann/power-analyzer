#ifndef HARMONICS_H
#define HARMONICS_H

#include "esp_http_server.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------- Trend-/Ringpuffer --------------------------------------- */
void store_frequency(float freq);          // von adc_fft.c aufgerufen

/* -------- HTTP / WebSocket-Handler -------------------------------- */
esp_err_t harmonics_handler(httpd_req_t *req);      // GET  /harmonics
esp_err_t ws_harmonics_handler(httpd_req_t *req);   // WS   /ws_harmonics

#ifdef __cplusplus
}
#endif
#endif /* HARMONICS_H */
