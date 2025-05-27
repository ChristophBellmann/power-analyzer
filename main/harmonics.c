#include <stdio.h>
#include <string.h>
#include <math.h>
#include "esp_log.h"
#include "esp_http_server.h"
#include "esp_dsp.h"
#include "config.h"
#include "harmonics.h"

/* ------------------------------------------------------------------ */
/*           Roh-Sample-Buffers aus adc_fft.c (extern)                 */
/* ------------------------------------------------------------------ */
extern int16_t buf_voltage[];      // Größe FFT_SIZE
extern int16_t buf_current[];

/* ------------------------------------------------------------------ */
/*                        Konstante Parameter                          */
/* ------------------------------------------------------------------ */
#define TAG             "HARMONICS"
#define HARMONIC_COUNT  40             // 1 … 40
#define FUND_FREQ       50.0f          // Grundton Netzklemme

/* ------------------------------------------------------------------ */
/*              Ringpuffer für Trend-Anzeige (Frequency)               */
/* ------------------------------------------------------------------ */
#define FREQ_STORAGE_SIZE 64
static float freq_store[FREQ_STORAGE_SIZE];
static int   freq_write = 0;
static int   freq_count = 0;

/* öffentlich – wird von adc_fft.c aufgerufen */
void store_frequency(float f)
{
    freq_store[freq_write] = f;
    freq_write = (freq_write + 1) % FREQ_STORAGE_SIZE;
    if (freq_count < FREQ_STORAGE_SIZE) freq_count++;
}

/* ------------------------------------------------------------------ */
/*                    Private FFT-Hilfspuffer                          */
/* ------------------------------------------------------------------ */
static float fft_input[FFT_SIZE * 2 * 2];   // 2 Kanäle, komplex
static float window_data[FFT_SIZE];

/* ------------------------------------------------------------------ */
/*                   JSON-Antwort senden (Helper)                      */
/* ------------------------------------------------------------------ */
static esp_err_t send_json(httpd_req_t *req,
                           const char *json,
                           size_t len)
{
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, json, len);
}

/* ------------------------------------------------------------------ */
/*                 FFT & Harmonische in JSON verpacken                */
/* ------------------------------------------------------------------ */
static void compute_harmonics_json(char *outbuf, size_t outsize)
{
    /* ----- 1. Mittelwert entfernen (DC) --------------------------- */
    float mean_v = 0.0f, mean_i = 0.0f;
    for (int n = 0; n < FFT_SIZE; n++) {
        mean_v += buf_voltage[n];
        mean_i += buf_current[n];
    }
    mean_v /= FFT_SIZE;
    mean_i /= FFT_SIZE;

    /* ----- 2. Hann-Fenster vorbereiten ---------------------------- */
    dsps_fft2r_init_fc32(NULL, CONFIG_DSP_MAX_FFT_SIZE);
    dsps_wind_hann_f32(window_data, FFT_SIZE);

    /* ----- 3. Samples * Fenster in fft_input schreiben ------------ */
    for (int n = 0; n < FFT_SIZE; n++) {
        float v = (buf_voltage[n] - mean_v) * window_data[n];
        float i = (buf_current[n] - mean_i) * window_data[n];

        /* Spannung – erstes FFT-Segment */
        fft_input[2*n]     = v;
        fft_input[2*n + 1] = 0.0f;

        /* Strom – zweites FFT-Segment */
        fft_input[2*(n + FFT_SIZE)]     = i;
        fft_input[2*(n + FFT_SIZE) + 1] = 0.0f;
    }

    /* ----- 4. Zwei FFTs nacheinander ------------------------------ */
    dsps_fft2r_fc32(fft_input, FFT_SIZE);
    dsps_bit_rev_fc32(fft_input, FFT_SIZE);
    dsps_cplx2reC_fc32(fft_input, FFT_SIZE);

    dsps_fft2r_fc32(fft_input + 2*FFT_SIZE, FFT_SIZE);
    dsps_bit_rev_fc32(fft_input + 2*FFT_SIZE, FFT_SIZE);
    dsps_cplx2reC_fc32(fft_input + 2*FFT_SIZE, FFT_SIZE);

    const float bin_w = SAMPLE_RATE / (float)FFT_SIZE;

    /* ----- 5. JSON schreiben -------------------------------------- */
    char *p = outbuf;
    #define APP(...) p += snprintf(p, outsize - (p - outbuf), __VA_ARGS__)

    APP("{");

    /* --- DC-Komponenten ------------------------------------------ */
    float dc_v = fabsf(fft_input[0]);
    float dc_i = fabsf(fft_input[2*FFT_SIZE]);
    APP("\"dc_v\":%.2f,\"dc_i\":%.2f,", dc_v, dc_i);

    /* --- Harmonische + THD-Summen -------------------------------- */
    float thd_sum_v = 0.0f, thd_sum_i = 0.0f;
    int   h1_idx    = (int)roundf(FUND_FREQ / bin_w);

    for (int k = 1; k <= HARMONIC_COUNT; k++) {
        int idx = (int)roundf((FUND_FREQ * k) / bin_w);
        if (idx < 1) idx = 1;
        if (idx > FFT_SIZE/2 - 1) idx = FFT_SIZE/2 - 1;

        /* Spannung */
        float mag_v = hypotf( fft_input[2*idx], fft_input[2*idx+1] );
        /* Strom     (zweiter Block) */
        float mag_i = hypotf( fft_input[2*(FFT_SIZE+idx)],
                              fft_input[2*(FFT_SIZE+idx)+1] );

        APP("\"h%d_v\":%.2f,\"h%d_i\":%.2f,", k, mag_v, k, mag_i);

        if (k >= 2) {
            thd_sum_v += mag_v * mag_v;
            thd_sum_i += mag_i * mag_i;
        }
    }

    /* --- THD (%) -------------------------------------------------- */
    float h1_v  = hypotf( fft_input[2*h1_idx],
                          fft_input[2*h1_idx+1] );
    float h1_i  = hypotf( fft_input[2*(FFT_SIZE+h1_idx)],
                          fft_input[2*(FFT_SIZE+h1_idx)+1] );

    float thd_v = (h1_v > 0) ? sqrtf(thd_sum_v)/h1_v * 100.0f : 0.0f;
    float thd_i = (h1_i > 0) ? sqrtf(thd_sum_i)/h1_i * 100.0f : 0.0f;

    APP("\"thd_v\":%.2f,\"thd_i\":%.2f}", thd_v, thd_i);
}

/* ------------------------------------------------------------------ */
/*                  HTTP-GET  /harmonics-Handler                       */
/* ------------------------------------------------------------------ */
esp_err_t harmonics_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "HTTP /harmonics");
    /* dynamisch, damit der httpd-Stack klein bleiben kann */
    char *json = malloc(2048);
    if (!json) return httpd_resp_send_500(req);
    compute_harmonics_json(json, 2048);
    esp_err_t ret = send_json(req, json, strlen(json));
    free(json);
    return ret;
}

/* ------------------------------------------------------------------ */
/*                  WebSocket-Handler  /ws_harmonics                   */
/* ------------------------------------------------------------------ */
esp_err_t ws_harmonics_handler(httpd_req_t *req)
{
    char *json = malloc(2048);
    if (!json) return ESP_ERR_NO_MEM;
    compute_harmonics_json(json, 2048);

// WS-Handler – letzte Zeilen ersetzen
    httpd_ws_frame_t frame = {
        .final  = true,
        .fragmented = false,
        .type   = HTTPD_WS_TYPE_TEXT,
        .payload = (uint8_t *)json,
        .len     = strlen(json)
    };
    esp_err_t ret = httpd_ws_send_frame(req, &frame);
    free(json);
    return ret;
}