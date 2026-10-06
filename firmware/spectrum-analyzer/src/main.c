#include "wifi.h"
#include "adc_fft.h"
#include "wav.h"
#include "esp_system.h"
#include "esp_log.h"
#include "fastdetect.h"
#include "spiffs_init.h"

static const char *TAG = "MainApp";

void monitor_free_ram_task(void *param) {
    while (1) {
        ESP_LOGI(TAG, "Free heap size: %u bytes", (unsigned int)esp_get_free_heap_size());
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main()
{
    esp_log_level_set("httpd_ws", ESP_LOG_DEBUG);
    esp_log_level_set("httpd_txrx", ESP_LOG_DEBUG);

    // Mount SPIFFS
    init_spiffs();

    // 1) Wi-Fi init
    wifi_init_sta();

    // 2) ADC / FFT init
    configure_adc_continuous();
    xTaskCreate(collect_adc_continuous_data, "ADC_Task", 4096, NULL, 5, NULL);

    // 3) Start the web server
    start_webserver();

    // 4) Start the fast-detect chunk task
    init_fastdetect_task();

    // Optionally monitor free RAM
    // xTaskCreate(monitor_free_ram_task, "monitor_free_ram_task", 2048, NULL, 5, NULL);
}
