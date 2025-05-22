#ifndef RECORDER_H
#define RECORDER_H

#include "esp_http_server.h"
#ifdef __cplusplus
extern "C" {
#endif

esp_err_t download_handler(httpd_req_t *req);

#ifdef __cplusplus
}
#endif
#endif // RECORDER_H
