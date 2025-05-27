#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_psram.h"
#include "esp_heap_caps.h"

#include "spiffs_init.h"
#include "config_manager.h"    // <<< hier ergänzt
#include "wifi.h"
#include "adc.h"
#include "recorder.h"
#include "webserver.h"

static const char *TAG = "PSRAM_TEST";

static void test_psram(void)
{
    // 1) Gesamte PSRAM-Größe abfragen
    size_t total = esp_psram_get_size();
    ESP_LOGI(TAG, "esp_psram_get_size(): %u KiB", total / 1024);

    // 2) PSRAM-Heap-Statistik
    size_t psram_total = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
    size_t psram_free  = heap_caps_get_free_size (MALLOC_CAP_SPIRAM);
    ESP_LOGI(TAG, "PSRAM-Heap: total %u KiB, free %u KiB",
             psram_total/1024, psram_free/1024);

    // 3) Große Allokation testen (z.B. 6 MiB)
    size_t want = 6 * 1024 * 1024;
    void *p = heap_caps_malloc(want, MALLOC_CAP_SPIRAM);
    if (p) {
        ESP_LOGI(TAG, "Erfolgreich %u KiB in PSRAM reserviert", want/1024);
        heap_caps_free(p);
    } else {
        ESP_LOGW(TAG, "Konnte %u KiB nicht allokieren", want/1024);
    }
}

void app_main(void)
{
    /* -------- Initialisierung ------------------------------------ */
    ESP_ERROR_CHECK(spiffs_init());             // SPIFFS mounten
    ESP_ERROR_CHECK(config_manager_load());     // Konfigurations-Defaults / YAML laden
    ESP_ERROR_CHECK(wifi_init());               // WLAN starten

    /* -------- PSRAM-Test (optional) ------------------------------ */
    test_psram();

    /* -------- Peripherie + Server -------------------------------- */
    configure_adc_continuous(ADC_MODE_SINGLE);  // ADC+DMA konfigurieren
    recorder_init();                            // interne Puffer initialisieren
    webserver_start();                          // HTTP/WebSocket-Server

    /* -------- ADC-Task starten ---------------------------------- */
    xTaskCreate(
        collect_adc_continuous_data,
        "adc_task",
        4096,
        NULL,
        5,
        NULL
    );

    /* -------- Leerlaufschleife ---------------------------------- */
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
