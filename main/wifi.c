#include "wifi.h"
#include "config.h"

#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <stdlib.h>

static const char *TAG = "WIFI";

/* ------------------------------------------------------------------ */
/*  Bekannte SSIDs  +  Kennwörter                                       */
typedef struct { char ssid[33]; char pass[65]; } known_ap_t;

static const known_ap_t s_known[] = {
    { WIFI_SSID_1, WIFI_PASS_1 },
#if WIFI_CRED_CT >= 2
    { WIFI_SSID_2, WIFI_PASS_2 },
#endif
#if WIFI_CRED_CT >= 3
    { WIFI_SSID_3, WIFI_PASS_3 },
#endif
#if WIFI_CRED_CT >= 4
    { WIFI_SSID_4, WIFI_PASS_4 },
#endif
};
#define KNOWN_AP_NUM  (sizeof(s_known)/sizeof(s_known[0]))

/* ------------------------------------------------------------------ *//* Forward-Deklarationen */
static void wifi_event_handler(void *, esp_event_base_t, int32_t, void *);
static void ip_logger_task(void *arg);

/* ------------------------------------------------------------------ */
static esp_err_t nvs_init(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    return err;
}

/* ------------------------------------------------------------------ */
esp_err_t wifi_init(void)
{
    ESP_ERROR_CHECK(nvs_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "WLAN-Stack bereit – starte ersten Scan …");

    wifi_start_scan();
    return ESP_OK;
}

/* ------------------------------------------------------------------ */
void wifi_start_scan(void)
{
    wifi_scan_config_t sc = { .show_hidden = true };
    ESP_ERROR_CHECK(esp_wifi_scan_start(&sc, false));
}

/* ---------------------------- Hotspot ------------------------------ */
static void start_hotspot(void)
{
    wifi_config_t ap = {0};
    strncpy((char *)ap.ap.ssid, WIFI_AP_FALLBACK_SSID, sizeof(ap.ap.ssid));
    strncpy((char *)ap.ap.password, WIFI_AP_FALLBACK_PWD, sizeof(ap.ap.password));
    ap.ap.ssid_len       = strlen(WIFI_AP_FALLBACK_SSID);
    ap.ap.channel        = WIFI_AP_CHANNEL;
    ap.ap.max_connection = 4;
    ap.ap.authmode       = (strlen(WIFI_AP_FALLBACK_PWD) >= 8)
                             ? WIFI_AUTH_WPA_WPA2_PSK
                             : WIFI_AUTH_OPEN;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap));
    ESP_LOGW(TAG, "Kein bekannter AP → Hotspot \"%s\" aktiv", WIFI_AP_FALLBACK_SSID);
}

/* ------------------------ Scan-Auswertung -------------------------- */
static void connect_to_known_ap(const wifi_ap_record_t *list, uint16_t num)
{
    for (uint16_t i = 0; i < num; ++i) {
        for (uint16_t k = 0; k < KNOWN_AP_NUM; ++k) {
            if (strcmp((const char *)list[i].ssid, s_known[k].ssid) == 0) {
                wifi_config_t sta = {0};
                strncpy((char *)sta.sta.ssid, s_known[k].ssid, sizeof(sta.sta.ssid));
                strncpy((char *)sta.sta.password, s_known[k].pass, sizeof(sta.sta.password));

                ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
                ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta));
                ESP_LOGI(TAG, "Verbinde zu \"%s\" (RSSI %d)", list[i].ssid, list[i].rssi);
                ESP_ERROR_CHECK(esp_wifi_connect());
                return;
            }
        }
    }
    /* kein Treffer */
    start_hotspot();
}

/* ------------------------ Event-Callback --------------------------- */
static void wifi_event_handler(void *arg, esp_event_base_t base,
                               int32_t id, void *data)
{
    if (base == WIFI_EVENT) {
        switch (id) {
        case WIFI_EVENT_SCAN_DONE: {
            uint16_t num = 0;
            esp_wifi_scan_get_ap_num(&num);
            wifi_ap_record_t *recs = calloc(num, sizeof(*recs));
            if (!recs) return;
            esp_wifi_scan_get_ap_records(&num, recs);
            connect_to_known_ap(recs, num);
            free(recs);
            break;
        }
        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_LOGW(TAG, "WLAN getrennt – neuer Scan …");
            wifi_start_scan();
            break;
        }
    }
    else if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *evt = (ip_event_got_ip_t *)data;
        ESP_LOGI(TAG, "Verbunden, IP: " IPSTR, IP2STR(&evt->ip_info.ip));

        static bool logger_started = false;
        if (!logger_started) {
            xTaskCreate(ip_logger_task, "ip_log", 2048, NULL, 3, NULL);
            logger_started = true;
        }
    }
}

/* ------------------------ 10-s-Logger-Task ------------------------- */
static void ip_logger_task(void *arg)
{
    esp_netif_ip_info_t ip;
    while (true) {
        if (esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), &ip) == ESP_OK) {
            ESP_LOGI(TAG, "IP-Status: " IPSTR, IP2STR(&ip.ip));
        }
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
