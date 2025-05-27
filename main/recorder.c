#include "recorder.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "RECORDER";

/* Definition der externen Puffer */
uint16_t buf_voltage[FFT_SIZE] = {0};
uint16_t buf_current[FFT_SIZE] = {0};

void recorder_init(void)
{
    ESP_LOGI(TAG, "Recorder initialisiert, FFT_SIZE = %d", FFT_SIZE);
    // Hier könnt ihr ADC-DMA/Task-Setup starten, wenn nötig
}

/*------------------------------------------------------------------------
 * send_scope_frame(): baut JSON { "volt":[...], "curr":[...] } und
 *   sendet es per WebSocket-Frame zurück.
 *------------------------------------------------------------------------*/
esp_err_t send_scope_frame(httpd_req_t *req)
{
    // Grob überschlagen ~ 12 Zeichen pro Sample
    static char json[FFT_SIZE * 12];
    char *p = json;

    p += sprintf(p, "{\"volt\":[");
    for (int i = 0; i < FFT_SIZE; ++i) {
        p += sprintf(p, "%u%s",
                     buf_voltage[i],
                     (i < FFT_SIZE - 1) ? "," : "");
    }
    p += sprintf(p, "],\"curr\":[");
    for (int i = 0; i < FFT_SIZE; ++i) {
        p += sprintf(p, "%u%s",
                     buf_current[i],
                     (i < FFT_SIZE - 1) ? "," : "");
    }
    p += sprintf(p, "]}");

    httpd_ws_frame_t frame = {
        .final      = true,
        .fragmented = false,
        .type       = HTTPD_WS_TYPE_TEXT,
        .payload    = (uint8_t*)json,
        .len        = strlen(json)
    };
    return httpd_ws_send_frame(req, &frame);
}

/*------------------------------------------------------------------------
 * download_handler(): schickt CSV-Daten über HTTP.
 *------------------------------------------------------------------------*/
esp_err_t download_handler(httpd_req_t *req)
{
    // Header + Spalten
    httpd_resp_set_type(req, "text/csv");
    httpd_resp_sendstr_chunk(req, "voltage,current\r\n");

    char line[64];
    for (int i = 0; i < FFT_SIZE; ++i) {
        float v = (buf_voltage[i] / 4095.0f) * VREF;
        float c = (buf_current[i] / 4095.0f) * IREF;
        int len = snprintf(line, sizeof(line),
                           "%.4f,%.4f\r\n", v, c);
        httpd_resp_send_chunk(req, line, len);
    }
    // Ende des Chunks
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}
