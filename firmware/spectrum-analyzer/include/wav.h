#ifndef WAV_H
#define WAV_H

#include "esp_http_server.h"

// HTTP-Handler für WAV-Download
esp_err_t wav_download_handler(httpd_req_t *req);

#endif // WAV_H
