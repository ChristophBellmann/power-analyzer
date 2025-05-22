// main.c

#include "wifi.h"
#include "adc_fft.h"
#include "recorder.h"      // statt wav.h
#include "esp_system.h"
#include "esp_log.h"
#include "fastdetect.h"
#include "spiffs_init.h"

static const char *TAG = "MainApp";

// (Optional) Task zur Überwachung des freien Heap-Speichers
void monitor_free_ram_task(void *param) {
    while (1) {
        ESP_LOGI(TAG, "Free heap size: %u bytes", (unsigned int)esp_get_free_heap_size());
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main()
{
    // Setze geeignetes Log-Level für HTTP/WebSocket
    esp_log_level_set("httpd_ws",  ESP_LOG_DEBUG);
    esp_log_level_set("httpd_txrx", ESP_LOG_DEBUG);
    esp_log_level_set("ADC_FFT",     ESP_LOG_INFO);
    esp_log_level_set("FASTDETECT",  ESP_LOG_INFO);
    esp_log_level_set("RECORDER",    ESP_LOG_INFO);

    // 0) SPIFFS mounten (für die Web-Assets)
    init_spiffs();

    // 1) Wi-Fi initialisieren (Station-Modus)
    wifi_init_sta();

    // 2) ADC-FFT initialisieren und ADC-Task starten
    configure_adc_continuous();
    xTaskCreate(
        collect_adc_continuous_data,
        "ADC_Task",
        4096,
        NULL,
        configMAX_PRIORITIES - 1,
        NULL
    );

    // 3) Webserver starten (Scope-, Fastdetect- und Download-Endpoints)
    start_webserver();

    // 4) Fast-Detect (Harmonics) Task starten
    init_fastdetect_task();

    // (Optional) RAM-Monitor
    // xTaskCreate(monitor_free_ram_task, "RAM_Monitor", 2048, NULL, 1, NULL);
}
