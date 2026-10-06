idf_component_register(
    SRCS adc_fft.c fastdetect.c http.c main.c spiffs_init.c wav.c wifi.c
    INCLUDE_DIRS "../include"
    REQUIRES esp-dsp esp_adc esp_http_server esp_wifi esp_event esp_netif nvs_flash spiffs driver
)
