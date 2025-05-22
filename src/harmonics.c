// harmonics.c

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "esp_log.h"
#include "esp_http_server.h"
#include "esp_dsp.h"
#include "config.h"
#include "harmonics.h"

// ADC-Buffers aus adc_fft.c
extern int16_t buf_voltage[];
extern int16_t buf_current[];

static const char *TAG = "HARMONICS";

// Anzahl der zu ermittelnden Harmonischen
#define HARMONIC_COUNT 40
// Fundamental-Frequenz (z.B. Netz 50 Hz)
#define FUND_FREQ      50.0f

// FFT-Puffer
static float fft_input[FFT_SIZE * 2];
static float window_data[FFT_SIZE];

// Hilfsfunktion: setzt JSON ins resp
static esp_err_t send_json(httpd_req_t *req, char *json, size_t len) {
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, json, len);
}

static void compute_harmonics_json(char *outbuf, size_t outsize) {
    // 1) DC-Entfernung + Window
    float mean_v = 0, mean_i = 0;
    for (int n = 0; n < FFT_SIZE; n++) {
        mean_v += buf_voltage[n];
        mean_i += buf_current[n];
    }
    mean_v /= FFT_SIZE;
    mean_i /= FFT_SIZE;

    dsps_fft2r_init_fc32(NULL, CONFIG_DSP_MAX_FFT_SIZE);
    dsps_wind_hann_f32(window_data, FFT_SIZE);

    for (int n = 0; n < FFT_SIZE; n++) {
        float v = (buf_voltage[n] - mean_v) * window_data[n];
        float i = (buf_current[n] - mean_i) * window_data[n];
        fft_input[2*n]     = v;
        fft_input[2*n + 1] = 0.0f;
    }
    // kopieren für zweiten Kanal
    for (int n = 0; n < FFT_SIZE; n++) {
        fft_input[2*(n+FFT_SIZE)]     = (buf_current[n] - mean_i) * window_data[n];
        fft_input[2*(n+FFT_SIZE) + 1] = 0.0f;
    }

    // FFT durchführen (erst Kanal V, dann Kanal I)
    dsps_fft2r_fc32(fft_input, FFT_SIZE);
    dsps_bit_rev_fc32(fft_input, FFT_SIZE);
    dsps_cplx2reC_fc32(fft_input, FFT_SIZE);

    dsps_fft2r_fc32(fft_input + FFT_SIZE*2, FFT_SIZE);
    dsps_bit_rev_fc32(fft_input + FFT_SIZE*2, FFT_SIZE);
    dsps_cplx2reC_fc32(fft_input + FFT_SIZE*2, FFT_SIZE);

    // Binbreite
    const float bin_width = SAMPLE_RATE / (float)FFT_SIZE;

    // JSON-Build
    char *p = outbuf;
    p += snprintf(p, outsize, "{");

    // DC-Komponenten (Bin 0)
    float dc_v = fabsf(fft_input[0]);
    float dc_i = fabsf(fft_input[FFT_SIZE*2]);
    p += snprintf(p + (p - outbuf), outsize - (p - outbuf),
                  "\"dc_v\":%.2f,\"dc_i\":%.2f,",
                  dc_v, dc_i);

    // Harmonische 1..40
    float thd_sum_v = 0, thd_sum_i = 0;
    for (int k = 1; k <= HARMONIC_COUNT; k++) {
        int idx = (int)round((FUND_FREQ * k) / bin_width);
        if (idx < 1) idx = 1;
        if (idx > FFT_SIZE/2 - 1) idx = FFT_SIZE/2 - 1;
        float mag_v = sqrtf(
            fft_input[2*idx]*fft_input[2*idx] +
            fft_input[2*idx+1]*fft_input[2*idx+1]
        );
        float mag_i = sqrtf(
            fft_input[FFT_SIZE*2 + 2*idx]*fft_input[FFT_SIZE*2 + 2*idx] +
            fft_input[FFT_SIZE*2 + 2*idx+1]*fft_input[FFT_SIZE*2 + 2*idx+1]
        );
        p += snprintf(p + (p - outbuf), outsize - (p - outbuf),
                      "\"h%d_v\":%.2f,\"h%d_i\":%.2f,",
                      k, mag_v, k, mag_i);
        if (k >= 2) {
            thd_sum_v += mag_v * mag_v;
            thd_sum_i += mag_i * mag_i;
        }
        if (p - outbuf >= outsize - 100) break;
    }

    // THD in Prozent
    // h1 magnitude
    float h1_v = sqrtf(fft_input[2* (int)round(FUND_FREQ/bin_width)]*
                       fft_input[2*(int)round(FUND_FREQ/bin_width)] +
                       fft_input[2*(int)round(FUND_FREQ/bin_width)+1]*
                       fft_input[2*(int)round(FUND_FREQ/bin_width)+1]);
    float h1_i = sqrtf(fft_input[FFT_SIZE*2 + 2*(int)round(FUND_FREQ/bin_width)]*
                       fft_input[FFT_SIZE*2 + 2*(int)round(FUND_FREQ/bin_width)] +
                       fft_input[FFT_SIZE*2 + 2*(int)round(FUND_FREQ/bin_width)+1]*
                       fft_input[FFT_SIZE*2 + 2*(int)round(FUND_FREQ/bin_width)+1]);
    float thd_v = (h1_v > 0) ? sqrtf(thd_sum_v)/h1_v * 100.0f : 0.0f;
    float thd_i = (h1_i > 0) ? sqrtf(thd_sum_i)/h1_i * 100.0f : 0.0f;

    p += snprintf(p + (p - outbuf), outsize - (p - outbuf),
                  "\"thd_v\":%.2f,\"thd_i\":%.2f",
                  thd_v, thd_i);

    p += snprintf(p + (p - outbuf), outsize - (p - outbuf), "}");
}

esp_err_t harmonics_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "/harmonics angefordert");
    char json[2048];
    compute_harmonics_json(json, sizeof(json));
    return send_json(req, json, strlen(json));
}

esp_err_t ws_harmonics_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "Harmonics WS Nachricht");
    // wie harmonics_handler, aber als WS-Frame
    char json[2048];
    compute_harmonics_json(json, sizeof(json));
    httpd_ws_frame_t resp;
    memset(&resp, 0, sizeof(resp));
    resp.type    = HTTPD_WS_TYPE_TEXT;
    resp.payload = (uint8_t*)json;
    resp.len     = strlen(json);
    return httpd_ws_send_frame(req, &resp);
}
