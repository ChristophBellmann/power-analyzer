#include <stdio.h>
#include <stdint.h>
#include "esp_log.h"
#include "esp_http_server.h"
#include "config.h"

// Externe ADC-Buffers (aus adc_fft.c)
extern int16_t buf_voltage[];
extern int16_t buf_current[];

static const char *TAG = "RECORDER";

esp_err_t download_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "Downloadrequested: %u samples at %u S/s",
             FFT_SIZE, SAMPLE_RATE);
    // Header senden
    uint32_t header[2] = { SAMPLE_RATE, FFT_SIZE };
    httpd_resp_set_type(req, "application/octet-stream");
    httpd_resp_set_hdr(req, "Content-Disposition",
                      "attachment; filename=data.bin");
    httpd_resp_send_chunk(req, (const char*)header, sizeof(header));

    // Rohdaten als float32 senden: (V, I) interleaved
    for (uint32_t i = 0; i < FFT_SIZE; i++) {
        float v = (buf_voltage[i] / 2048.0f) * VREF;  // in Volt
        float c = (buf_current[i] / 2048.0f) * IREF;  // in Ampere
        float pair[2] = { v, c };
        httpd_resp_send_chunk(req, (const char*)pair, sizeof(pair));
    }

    // Ende des Chunked-Transfers
    return httpd_resp_send_chunk(req, NULL, 0);
}
