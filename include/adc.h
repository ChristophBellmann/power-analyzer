#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Betriebsmodi ------------------------------------------------------- */
typedef enum {
    ADC_MODE_SINGLE = 0,          /* Alias: Einzel-/DC-Messung  */
    ADC_MODE_DC     = ADC_MODE_SINGLE,
    ADC_MODE_AC,
    ADC_MODE_HARMONICS
} adc_mode_t;

/* Getter / Setter ---------------------------------------------------- */
adc_mode_t adc_get_mode(void);
void       adc_change_mode(adc_mode_t new_mode);

/* Kontinuierlicher DMA-Betrieb -------------------------------------- */
void configure_adc_continuous(adc_mode_t mode);
void collect_adc_continuous_data(void *arg);   /* FreeRTOS-Task */

#ifdef __cplusplus
}
#endif
