#include "spiffs_init.h"
#include "esp_spiffs.h"
#include "esp_log.h"

static const char *TAG = "SPIFFS";

esp_err_t spiffs_init(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path              = "/spiffs",
        .partition_label        = "spiffs",   // Name in partitions.csv
        .max_files              = 5,
        .format_if_mount_failed = false
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPIFFS mount failed (%s)", esp_err_to_name(ret));
        return ret;
    }

    size_t total = 0, used = 0;
    if (esp_spiffs_info(conf.partition_label, &total, &used) == ESP_OK)
        ESP_LOGI(TAG, "SPIFFS: %.1f KiB total, %.1f KiB used",
                 total / 1024.0, used / 1024.0);
    return ESP_OK;
}
