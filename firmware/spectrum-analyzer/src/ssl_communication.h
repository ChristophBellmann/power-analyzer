#include <stdio.h>
#include <string.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_tls.h"
#include "nvs_flash.h"
#include "esp_timer.h"
#include "esp_crt_bundle.h"
#include "mbedtls/debug.h"

#define WIFI_SSID "Pixel_cb"
#define WIFI_PASS "Lassmichrein!"
#define INTERNET_TEST_URL "https://duckduckgo.com/"
#define WEBHOOK_URL "https://e725-2001-638-102-26-f8db-13b5-4ee8-2b98.ngrok-free.app/webhook/"

static const char *TAG = "ESP32_APP";
static bool wifi_connected = false;

// Debugging-Funktion für mbedTLS
void mbedtls_debug(void *ctx, int level, const char *file, int line, const char *str) {
    ESP_LOGI("mbedTLS", "%s:%d %s", file, line, str);
}

// Wi-Fi Event Handler
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
        ESP_LOGI(TAG, "Reconnecting to Wi-Fi...");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        wifi_connected = true;
        ESP_LOGI(TAG, "Wi-Fi connected.");
    }
}

// Prüft die Internetverbindung
void check_internet_connectivity() {
    esp_http_client_config_t config = {
        .url = INTERNET_TEST_URL,
        .method = HTTP_METHOD_GET,
        .crt_bundle_attach = esp_crt_bundle_attach, // Verwendet die globale CA-Bundle-Verwaltung
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int status_code = esp_http_client_get_status_code(client);
        ESP_LOGI(TAG, "Internet Check Status Code: %d", status_code);
        if (status_code == 200) {
            ESP_LOGI(TAG, "Internet connectivity verified.");
        } else {
            ESP_LOGW(TAG, "Unexpected status code: %d", status_code);
        }
    } else {
        ESP_LOGE(TAG, "Internet check failed: %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}

// Sendet sinusförmige Daten an den Webhook
void send_sinusoidal_data(float value) {
    char post_data[64];
    snprintf(post_data, sizeof(post_data), "{\"value\": %.2f}", value);

    esp_http_client_config_t config = {
        .url = WEBHOOK_URL,
        .method = HTTP_METHOD_POST,
        .crt_bundle_attach = esp_crt_bundle_attach, // Verwendet die globale CA-Bundle-Verwaltung
        .timeout_ms = 5000, // Timeout erhöhen
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP POST Successful. Status: %d", esp_http_client_get_status_code(client));
    } else {
        ESP_LOGE(TAG, "HTTP POST Failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}

// Sinusförmige Daten generieren und senden
void sinusoidal_task(void *pvParameters) {
    const float frequency = 1.0;      // Frequenz in Hz
    const float amplitude = 20000.0; // Amplitude
    const int update_rate = 30;      // Aktualisierungsrate (30 Mal pro Sekunde)
    const int delay_ms = 1000 / update_rate;

    ESP_LOGI(TAG, "Starting sinusoidal data transmission...");
    uint64_t start_time = esp_timer_get_time();
    while (true) {
        float elapsed_time = (esp_timer_get_time() - start_time) / 1000000.0;
        float value = amplitude * sin(2 * M_PI * frequency * elapsed_time);

        ESP_LOGI(TAG, "Generated Value: %.2f", value);
        send_sinusoidal_data(value);

        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}

// Initialisiert das Wi-Fi
void wifi_init() {
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    esp_wifi_start();
}

// Hauptprogramm
void app_main() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Aktiviert die globale CA-Bundle-Verwaltung
    ESP_ERROR_CHECK(esp_crt_bundle_attach(NULL));

    // Aktiviert mbedTLS-Debugging
    esp_tls_set_global_ca_store(NULL);
    esp_tls_set_global_debug_callback(mbedtls_debug);

    // Wi-Fi initialisieren
    wifi_init();

    // Warte auf WLAN-Verbindung
    while (!wifi_connected) {
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    // Internetverbindung überprüfen
    check_internet_connectivity();

    // Starte die Aufgabe zur Übertragung der sinusförmigen Daten
    xTaskCreate(sinusoidal_task, "sinusoidal_task", 4096, NULL, 5, NULL);
}
