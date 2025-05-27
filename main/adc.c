#include "adc.h"
#include "esp_log.h"
#include "driver/adc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "ADC";
static adc_mode_t   s_mode = ADC_MODE_SINGLE;

/* ---------- Basis-Getter / Setter ---------------------------------- */
adc_mode_t adc_get_mode(void)
{
    return s_mode;
}

void adc_change_mode(adc_mode_t new_mode)
{
    if (new_mode == s_mode) return;

    /* TODO: Hardware-Reconfigure hier … */
    ESP_LOGI(TAG, "Mode %d → %d", s_mode, new_mode);
    s_mode = new_mode;
}

/* ---------- Kontinuierlicher DMA-/I2S-ADC-Modus -------------------- */
void configure_adc_continuous(adc_mode_t mode)
{
    adc_change_mode(mode);

    /* Minimal-Stub: echte DMA-Konfiguration nach Bedarf ergänzen */
    ESP_LOGI(TAG, "Continuous-ADC konfiguriert (Mode %d)", mode);
}

void collect_adc_continuous_data(void *arg)
{
    ESP_LOGI(TAG, "ADC-Task läuft");
    while (true) {
        /* TODO: neue Daten einsammeln, in Puffer ablegen … */
        vTaskDelay(pdMS_TO_TICKS(100));      /* Platzhalter */
    }
}
