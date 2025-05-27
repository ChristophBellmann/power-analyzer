#include "recorder.h"
#include "esp_log.h"
#include <stdio.h>

/* ------------------------------------------------------------- */
static const char *TAG = "RECORDER";

/* Öffentliche Puffer (Definition) ------------------------------ */
uint16_t buf_voltage[FFT_SIZE] = {0};
uint16_t buf_current[FFT_SIZE] = {0};

/* ------------------------------------------------------------- */
void recorder_init(void)
{
    ESP_LOGI(TAG, "Recorder initialisiert, FFT_SIZE = %d", FFT_SIZE);
    /* Hier könntest du ADC-DMA oder Task-Erzeugung starten */
}

/* ------------------------------------------------------------- */
void send_scope_frame(void)
/* Baut ein minimales JSON-Objekt:
   { "volt":[v0,v1,...], "curr":[c0,c1,...] }                */
{
    char json[FFT_SIZE * 12];          /* grob kalkulierte Größe   */
    char *p = json;

    p += sprintf(p, "{\"volt\":[");
    for (int i = 0; i < FFT_SIZE; ++i)
        p += sprintf(p, "%u%s", buf_voltage[i], (i < FFT_SIZE - 1) ? "," : "");

    p += sprintf(p, "],\"curr\":[");
    for (int i = 0; i < FFT_SIZE; ++i)
        p += sprintf(p, "%u%s", buf_current[i], (i < FFT_SIZE - 1) ? "," : "");

    p += sprintf(p, "]}");

    /* TODO: an WebSocket / TCP streamen statt nur loggen */
    ESP_LOGD(TAG, "Scope-Frame: %s", json);
}

/* ------------------------------------------------------------- */
esp_err_t download_handler(httpd_req_t *req)
/* Liefert eine einfache CSV-Datei: voltage,current pro Zeile   */
{
    httpd_resp_set_type(req, "text/csv");
    httpd_resp_sendstr_chunk(req, "voltage,current\r\n");

    char line[64];
    for (int i = 0; i < FFT_SIZE; ++i) {
        float v = (buf_voltage[i] / 4095.0f) * VREF;
        float c = (buf_current[i] / 4095.0f) * IREF;
        int len = snprintf(line, sizeof(line), "%.4f,%.4f\r\n", v, c);
        httpd_resp_send_chunk(req, line, len);
    }
    httpd_resp_send_chunk(req, NULL, 0);   /* Transfer beenden */
    return ESP_OK;
}
