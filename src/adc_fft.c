#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_adc/adc_continuous.h"
#include "esp_log.h"
#include "esp_dsp.h"
#include "config.h"
#include "freertos/semphr.h"
#include "fastdetect.h"

static const char *TAG = "ADC_FFT";

#define BUF_LEN FFT_SIZE
__attribute__((aligned(16))) int16_t buf_voltage[BUF_LEN];
__attribute__((aligned(16))) int16_t buf_current[BUF_LEN];
__attribute__((aligned(16))) int16_t adc_buffer[BUF_LEN * 2];
adc_continuous_handle_t adc_handle = NULL;

void configure_adc_continuous() {
    adc_continuous_handle_cfg_t cfg = {
        .max_store_buf_size = (BUF_LEN*2)*sizeof(int16_t),
        .conv_frame_size   = BUF_LEN*2,
    };
    ESP_ERROR_CHECK(adc_continuous_new_handle(&cfg, &adc_handle));
    adc_continuous_config_t c2 = {
        .sample_freq_hz = 200000,
        .conv_mode      = ADC_CONV_BOTH_UNIT_1,
        .format         = ADC_DIGI_OUTPUT_FORMAT_TYPE1,
        .pattern_num    = 2,
    };
    adc_digi_pattern_config_t pattern[2] = {
      { .atten=ADC_ATTEN_DB_11, .channel=ADC_CHANNEL_0, .unit=ADC_UNIT_1, .bit_width=ADC_BITWIDTH_12 },
      { .atten=ADC_ATTEN_DB_11, .channel=ADC_CHANNEL_1, .unit=ADC_UNIT_1, .bit_width=ADC_BITWIDTH_12 }
    };
    c2.adc_pattern = pattern;
    ESP_ERROR_CHECK(adc_continuous_config(adc_handle, &c2));
    ESP_LOGI(TAG, "ADC 2-Kanal 200kSPS ready");
}

void collect_adc_continuous_data() {
    ESP_ERROR_CHECK(adc_continuous_start(adc_handle));
    size_t bytes_read = 0;
    while (1) {
        ESP_ERROR_CHECK(adc_continuous_read(adc_handle,
            (uint8_t*)adc_buffer, sizeof(adc_buffer),
            (uint32_t*)&bytes_read, portMAX_DELAY));
        int samples = bytes_read/sizeof(int16_t)/2;
        for (int i = 0; i < samples; i++) {
            buf_voltage[i] = adc_buffer[2*i];
            buf_current[i] = adc_buffer[2*i+1];
        }
        vTaskDelay(pdMS_TO_TICKS(ADC_TASK_DELAY_MS));
    }
}
