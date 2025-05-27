#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi.h"
#include "adc.h"
#include "webserver.h"
#include "recorder.h"

void app_main(void)
{
    /* --- Peripherie hochfahren ------------------------------------ */
    ESP_ERROR_CHECK(wifi_init());     /* neue API */                            /* WLAN up               */
    configure_adc_continuous(ADC_MODE_SINGLE);     /* ADC-DMA vorbereiten   */
    recorder_init();                               /* Puffer / Logik        */
    webserver_start();                             /* HTTP-Server & API     */

    /* ADC-Task starten --------------------------------------------- */
    xTaskCreate(collect_adc_continuous_data, "adc_task",
                4096, NULL, 5, NULL);

    /* Hauptthread untätig lassen ----------------------------------- */
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
