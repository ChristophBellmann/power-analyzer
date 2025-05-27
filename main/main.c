#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "wifi.h"
#include "adc.h"
#include "recorder.h"
#include "webserver.h"
#include "spiffs_init.h"

void app_main(void)
{
    /* -------- Initialisierung ------------------------------------ */
    ESP_ERROR_CHECK(spiffs_init());                    // Dateisystem
    ESP_ERROR_CHECK(wifi_init());                      // WLAN

    configure_adc_continuous(ADC_MODE_SINGLE);         // ADC-DMA
    recorder_init();                                   // Puffer / Logik
    webserver_start();                                 // HTTP-Server

    /* -------- ADC-Task ------------------------------------------- */
    xTaskCreate(collect_adc_continuous_data,
                "adc_task", 4096, NULL, 5, NULL);

    /* -------- Leerlaufschleife ---------------------------------- */
    for (;;) vTaskDelay(pdMS_TO_TICKS(1000));
}
