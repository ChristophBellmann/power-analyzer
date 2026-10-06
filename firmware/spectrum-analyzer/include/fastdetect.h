#ifndef FASTDETECT_H
#define FASTDETECT_H

#include "esp_err.h"
#include "esp_http_server.h"

#ifdef __cplusplus
extern "C" {
#endif

// Speichert eine Frequenzmessung im ringförmigen Puffer.
void store_frequency(float freq);

// Initialisiert die Fastdetect-Task.
void init_fastdetect_task(void);

// HTTP-Handler für /fastdetect.
esp_err_t fastdetect_handler(httpd_req_t *req);

// WebSocket-Handler für Fastdetect-Daten.
esp_err_t ws_handler(httpd_req_t *req);

// Deklaration für build_chunk_json, damit diese Funktion in anderen Modulen (z. B. http.c) bekannt ist.
void build_chunk_json(char *outbuf, size_t outsize);

#ifdef __cplusplus
}
#endif

#endif // FASTDETECT_H
