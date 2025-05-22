// harmonics.h

#ifndef HARMONICS_H
#define HARMONICS_H

#include "esp_err.h"
#include "esp_http_server.h"
#include <stddef.h>

/**
 * @brief HTTP-Handler für Harmonics-/THD-Ausgabe als JSON.
 * 
 * Gibt ein JSON-Objekt zurück mit:
 * - dc_v, dc_i
 * - h1_v, …, h40_v
 * - h1_i, …, h40_i
 * - thd_v, thd_i (%)
 */
esp_err_t harmonics_handler(httpd_req_t *req);

/**
 * @brief WebSocket-Handler für Harmonics-/THD-Daten.
 * 
 * Liefert das gleiche JSON wie harmonics_handler.
 */
esp_err_t ws_harmonics_handler(httpd_req_t *req);

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif // HARMONICS_H
