#include "adc.h"
#include "recorder.h"                    /* buf_voltage / buf_current    */
#include "esp_log.h"
#include "esp_adc/adc_continuous.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

static const char *TAG   = "ADC";
static adc_mode_t   s_mode = ADC_MODE_SINGLE;

/* ------------------------------------------------------------------ */
/*                    Öffentliche Variablen                           */
/* ------------------------------------------------------------------ */
adc_continuous_handle_t adc_handle = NULL;

/* interner DMA-Zwischenpuffer (max. 2 Kanäle)                        */
static int16_t adc_buffer[FFT_SIZE * 2] __attribute__((aligned(16)));

/* ------------------------------------------------------------------ */
/*                       Mode Getter / Setter                         */
/* ------------------------------------------------------------------ */
adc_mode_t adc_get_mode(void)
{
    return s_mode;
}

void adc_change_mode(adc_mode_t new_mode)
{
    if (new_mode == s_mode) return;
    ESP_LOGI(TAG, "ADC-Modus %d → %d", s_mode, new_mode);
    s_mode = new_mode;
}

/* ------------------------------------------------------------------ */
/*                 Continuous-ADC (DMA) konfigurieren                 */
/* ------------------------------------------------------------------ */
esp_err_t configure_adc_continuous(adc_mode_t mode)
{
    /* vorhandenes Handle ggf. schließen ---------------------------- */
    if (adc_handle) {
        adc_continuous_stop(adc_handle);
        adc_continuous_deinit(adc_handle);
        adc_handle = NULL;
    }
    adc_change_mode(mode);

    /* 1) Handle ----------------------------------------------------- */
    adc_continuous_handle_cfg_t h_cfg = {
        .max_store_buf_size = FFT_SIZE * 2 * sizeof(int16_t),
        .conv_frame_size    = FFT_SIZE * ((mode == ADC_MODE_SINGLE) ? 1 : 2),
    };
    ESP_ERROR_CHECK(adc_continuous_new_handle(&h_cfg, &adc_handle));

    /* 2) Konvertierungs-Config ------------------------------------- */
    adc_digi_pattern_config_t patt[2] = {
        { .atten     = ADC_ATTEN_DB_12,
          .channel   = ADC_CHANNEL_0,
          .unit      = ADC_UNIT_1,
          .bit_width = ADC_BITWIDTH_12 },
        { .atten     = ADC_ATTEN_DB_12,
          .channel   = ADC_CHANNEL_1,
          .unit      = ADC_UNIT_1,
          .bit_width = ADC_BITWIDTH_12 }
    };

    adc_continuous_config_t c_cfg = {
        .sample_freq_hz = SAMPLE_RATE,
        .conv_mode      = ADC_CONV_SINGLE_UNIT_1,     /* immer Unit-1       */
        .format         = ADC_DIGI_OUTPUT_FORMAT_TYPE1,
        .pattern_num    = (mode == ADC_MODE_SINGLE) ? 1 : 2,
        .adc_pattern    = patt
    };
    ESP_ERROR_CHECK(adc_continuous_config(adc_handle, &c_cfg));

    ESP_LOGI(TAG, "ADC ready: %.1f kS/s, %s-Mode",
             SAMPLE_RATE / 1000.0f,
             (mode == ADC_MODE_SINGLE) ? "1-ch" : "2-ch");

    return ESP_OK;
}

/* ------------------------------------------------------------------ */
/*               FreeRTOS-Task: Daten einsammeln + speichern           */
/* ------------------------------------------------------------------ */
void collect_adc_continuous_data(void *arg)
{
    /* --- Simulation anstelle echter DMA-Samples ------------------- */
    const float w = 2.0f * (float)M_PI * 50.0f / (float)SAMPLE_RATE; /* 50 Hz */

    ESP_LOGI(TAG, "ADC-Task gestartet (Simulationsdaten)");
    while (true) {

        /* ----------- 1) Testdaten in globale Puffer schreiben ------ */
        for (int n = 0; n < FFT_SIZE; ++n) {
            buf_voltage[n] = 2048 + (int)(1500 * sinf(w * n));                    /* gelb  */
            buf_current[n] = (adc_get_mode() == ADC_MODE_INTERLEAVED)
                             ? 2048 + (int)(1000 * sinf(w * n + M_PI/4.0f))       /* cyan  */
                             : 0;                                                 /* grau  */
        }

        /* ----------- 2) (später) echte DMA-Samples lesen ----------- */
        /*
        size_t bytes = 0;
        ESP_ERROR_CHECK(adc_continuous_read(adc_handle,
                                            (uint8_t*)adc_buffer,
                                            sizeof(adc_buffer),
                                            &bytes,
                                            portMAX_DELAY));

        int16_t *p = adc_buffer;
        for (int i = 0; i < FFT_SIZE; ++i) {
            if (s_mode == ADC_MODE_SINGLE) {
                buf_voltage[i] = *p++;
                buf_current[i] = 0;
            } else {                                    // interleaved
                buf_voltage[i] = *p++;
                buf_current[i] = *p++;
            }
        }
        */

        vTaskDelay(pdMS_TO_TICKS(ADC_TASK_DELAY_MS));
    }
}
