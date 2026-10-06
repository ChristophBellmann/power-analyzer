#include <stdio.h>
#include "esp_spiffs.h"
#include "esp_log.h"
#include "spiffs_init.h"

static const char *TAG = "SPIFFS_INIT";

void init_spiffs(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = false
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format SPIFFS");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to init SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    if (esp_spiffs_info(NULL, &total, &used) == ESP_OK) {
        ESP_LOGI(TAG, "SPIFFS mounted: %d KiB total, %d KiB used", total/1024, used/1024);
    } else {
        ESP_LOGE(TAG, "Failed to get SPIFFS info");
    }
}
