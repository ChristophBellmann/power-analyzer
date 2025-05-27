#pragma once
#include <stdint.h>
#include "config.h"                     /* FFT_SIZE, SAMPLE_RATE … */
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---------- Betriebs-/Aufzeichnungs­modi ------------------------- */
typedef enum {
    ADC_MODE_SINGLE = 0,     /* nur CH-0                                       */
    ADC_MODE_INTERLEAVED     /* CH-0 + CH-1  (gleiche Sample-Rate pro Kanal)   */
} adc_mode_t;

/* ---------- Öffentliche API -------------------------------------- */
adc_mode_t       adc_get_mode(void);
void             adc_change_mode(adc_mode_t new_mode);

esp_err_t        configure_adc_continuous(adc_mode_t mode);
void             collect_adc_continuous_data(void *arg);

/* ---------- Weitergabe des Treiber-Handles (optional) ------------ */
#include "esp_adc/adc_continuous.h"
extern adc_continuous_handle_t adc_handle;

#ifdef __cplusplus
}
#endif
