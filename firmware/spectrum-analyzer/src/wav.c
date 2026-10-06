#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_adc/adc_continuous.h"
#include "esp_http_server.h"
#include "config.h"
#include "adc_fft.h" // Use extern declarations
#include "wav.h"

static const char *TAG = "WAV";

#define HEADER_SIZE 44

// Function to create WAV header
void generate_wav_header(uint8_t *header, uint32_t data_size) {
    uint32_t sample_rate = SAMPLE_RATE;
    uint16_t bits_per_sample = 16;
    uint16_t num_channels = 1;
    uint32_t byte_rate = sample_rate * num_channels * (bits_per_sample / 8);
    uint16_t block_align = num_channels * (bits_per_sample / 8);

    memcpy(header, "RIFF", 4);                            // ChunkID
    *((uint32_t *)(header + 4)) = 36 + data_size;         // ChunkSize
    memcpy(header + 8, "WAVE", 4);                       // Format
    memcpy(header + 12, "fmt ", 4);                      // Subchunk1ID
    *((uint32_t *)(header + 16)) = 16;                   // Subchunk1Size
    *((uint16_t *)(header + 20)) = 1;                    // AudioFormat (PCM)
    *((uint16_t *)(header + 22)) = num_channels;         // NumChannels
    *((uint32_t *)(header + 24)) = sample_rate;          // SampleRate
    *((uint32_t *)(header + 28)) = byte_rate;            // ByteRate
    *((uint16_t *)(header + 32)) = block_align;          // BlockAlign
    *((uint16_t *)(header + 34)) = bits_per_sample;      // BitsPerSample
    memcpy(header + 36, "data", 4);                     // Subchunk2ID
    *((uint32_t *)(header + 40)) = data_size;            // Subchunk2Size
}

esp_err_t wav_download_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "Playback/Download request received. Generating new WAV file...");

    // Collect new ADC data
    save_adc_data();

    // Prepare WAV file
    uint32_t data_size = NUM_BUFFERS * FFT_SIZE * sizeof(int16_t);
    uint8_t wav_header[HEADER_SIZE];
    generate_wav_header(wav_header, data_size);

    // Stream the WAV header
    httpd_resp_set_type(req, "audio/wav");
    httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=adc_data.wav");
    esp_err_t res = httpd_resp_send_chunk(req, (const char *)wav_header, HEADER_SIZE);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "Failed to send WAV header");
        return res;
    }

    // Stream ADC data buffer-by-buffer
    for (int i = 0; i < NUM_BUFFERS; i++) {
        res = httpd_resp_send_chunk(req, (const char *)collected_data[i], FFT_SIZE * sizeof(int16_t));
        if (res != ESP_OK) {
            ESP_LOGE(TAG, "Failed to send buffer %d", i);
            return res;
        }
    }

    // End the HTTP chunked response
    return httpd_resp_send_chunk(req, NULL, 0);
}


