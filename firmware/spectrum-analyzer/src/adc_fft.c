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
#include "fastdetect.h"  // Für store_frequency()

static const char *TAG = "ADC_FFT";

// Pufferspeicher – 16-Byte-Ausrichtung (Optimierung)
__attribute__((aligned(16))) int16_t adc_buffer[FFT_SIZE];
__attribute__((aligned(16))) float fft_input[FFT_SIZE * 2];
__attribute__((aligned(16))) float window[FFT_SIZE];
__attribute__((aligned(16))) float detrended_data[FFT_SIZE];

// ADC-Handle
adc_continuous_handle_t adc_handle = NULL;

// Globale Variablen: Hauptfrequenz (LF-Bereich) und Magnitude
float main_frequency = 0.0;
float max_magnitude = 0.0;

// Ringpuffer für gesammelte ADC-Daten
int16_t collected_data[NUM_BUFFERS][FFT_SIZE];
int current_buffer_index = 0;

// Semaphore für den ADC-Zugriff in save_adc_data
static SemaphoreHandle_t adc_semaphore = NULL;

/**
 * Konfiguriert den ADC im Continuous-Modus.
 */
void configure_adc_continuous() {
    adc_continuous_handle_cfg_t continuous_cfg = {
        .max_store_buf_size = FFT_SIZE * sizeof(int16_t),
        .conv_frame_size = FFT_SIZE,
    };
    ESP_ERROR_CHECK(adc_continuous_new_handle(&continuous_cfg, &adc_handle));

    adc_continuous_config_t adc_config = {
        .sample_freq_hz = SAMPLE_RATE,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,
        .format = ADC_DIGI_OUTPUT_FORMAT_TYPE1,
        .pattern_num = 1,
    };

    adc_digi_pattern_config_t adc_pattern[1] = {
        {
            .atten = ADC_ATTEN,
            .channel = ADC_CHANNEL,
            .unit = ADC_UNIT_1,
            .bit_width = ADC_BITWIDTH_12,
        }
    };

    adc_config.adc_pattern = adc_pattern;
    ESP_ERROR_CHECK(adc_continuous_config(adc_handle, &adc_config));
    ESP_LOGI(TAG, "ADC continuous mode configured");
}

/**
 * Führt die FFT aus und sucht im LF-Bereich (zwischen LF_LOW_FREQ und LF_HIGH_FREQ) nach
 * einem zusammenhängenden Frequenzsegment, dessen integrierte Amplitude über ein gleitendes Fenster
 * (definiert durch WINDOW_BANDWIDTH_HZ) maximal ist.
 *
 * Wird die integrierte Amplitude als zu niedrig befunden (unter MIN_TOTAL_AMPLITUDE),
 * wird die Hauptfrequenz auf **1** gesetzt – so signalisiert der Tuner, dass es leise ist.
 *
 * Der neue Frequenzwert wird zudem mittels Rate Limiting (maximal RATE_LIMIT_MAX_JUMP_HZ Sprung)
 * begrenzt.
 */
void perform_fft() {

    #if ENABLE_ADC_FFT_LOGS
        ESP_LOGI(TAG, "Performing FFT...");
    #endif

    // Berechne den Mittelwert (DC) und entferne diesen
    float mean = 0.0f;
    for (int i = 0; i < FFT_SIZE; i++) {
        mean += adc_buffer[i];
    }
    mean /= FFT_SIZE;

    for (int i = 0; i < FFT_SIZE; i++) {
        detrended_data[i] = adc_buffer[i] - mean;
    }

    // FFT initialisieren und Hann-Fenster anwenden
    dsps_fft2r_init_fc32(NULL, CONFIG_DSP_MAX_FFT_SIZE);
    dsps_wind_hann_f32(window, FFT_SIZE);
    for (int i = 0; i < FFT_SIZE; i++) {
        fft_input[i * 2]     = detrended_data[i] * window[i]; // Realteil
        fft_input[i * 2 + 1] = 0.0f;                           // Imaginärteil
    }

    // FFT durchführen
    dsps_fft2r_fc32(fft_input, FFT_SIZE);
    dsps_bit_rev_fc32(fft_input, FFT_SIZE);
    dsps_cplx2reC_fc32(fft_input, FFT_SIZE);

    // High-Pass Filter: Setze alle Bins unter 20 Hz auf Null
    const float bin_width = SAMPLE_RATE / (float)FFT_SIZE;
    int cutoff_bin = (int)(20.0f / bin_width);
    for (int i = 0; i < cutoff_bin; i++) {
        fft_input[i * 2] = 0.0f;
        fft_input[i * 2 + 1] = 0.0f;
    }

    // Definiere den LF-Bereich anhand von LF_LOW_FREQ und LF_HIGH_FREQ
    int lf_low_index = (int)ceil(LF_LOW_FREQ / bin_width);
    int lf_high_index = (int)floor(LF_HIGH_FREQ / bin_width);
    if (lf_low_index < cutoff_bin) {
        lf_low_index = cutoff_bin;
    }
    if (lf_high_index > FFT_SIZE / 2 - 1) {
        lf_high_index = FFT_SIZE / 2 - 1;
    }

    // Berechne die Magnituden (Amplitude) für die relevanten Bins im LF-Bereich
    int num_bins = lf_high_index - lf_low_index + 1;
    float *magnitudes = malloc(num_bins * sizeof(float));
    if (magnitudes == NULL) {
        ESP_LOGE(TAG, "Memory allocation for magnitudes failed!");
        return;
    }
    for (int i = 0; i < num_bins; i++) {
        int bin_index = lf_low_index + i;
        magnitudes[i] = sqrt(fft_input[bin_index * 2] * fft_input[bin_index * 2] +
                             fft_input[bin_index * 2 + 1] * fft_input[bin_index * 2 + 1]);
    }

    // Fensterbreite in Hz, definiert durch WINDOW_BANDWIDTH_HZ
    int seg_bins = (int)round(WINDOW_BANDWIDTH_HZ / bin_width);
    if (seg_bins < 1) seg_bins = 1;
    if (seg_bins > num_bins) seg_bins = num_bins;

    // Suche nach dem Fenster (im LF-Bereich) mit der höchsten integrierten Amplitude
    float max_segment_sum = 0.0f;
    int best_segment_start = 0;
    for (int start = 0; start <= num_bins - seg_bins; start++) {
        float segment_sum = 0.0f;
        for (int j = 0; j < seg_bins; j++) {
            segment_sum += magnitudes[start + j];
        }
        if (segment_sum > max_segment_sum) {
            max_segment_sum = segment_sum;
            best_segment_start = start;
        }
    }

    // Wird die integrierte Amplitude als zu niedrig befunden, setze Hauptfrequenz auf 1.
    if (max_segment_sum < MIN_TOTAL_AMPLITUDE) {
        main_frequency = 1.0f;
        max_magnitude = max_segment_sum;
        #if ENABLE_ADC_FFT_LOGS
            ESP_LOGI(TAG, "Amplitude too low: %.2f. Main frequency set to 1.", max_segment_sum);
        #endif
        free(magnitudes);
        store_frequency(main_frequency);
        return;
    }

    // Berechne den Bin-Mittelpunkt des besten Fensters
    float center_bin = lf_low_index + best_segment_start + (seg_bins / 2.0f);
    float new_frequency = center_bin * bin_width;  // in Hz

    // Rate Limiting: Erlaube maximal RATE_LIMIT_MAX_JUMP_HZ Frequenzsprung pro Zyklus
    static float prev_frequency = 1.0f;
    if (fabs(new_frequency - prev_frequency) > RATE_LIMIT_MAX_JUMP_HZ) {
        if (new_frequency > prev_frequency)
            new_frequency = prev_frequency + RATE_LIMIT_MAX_JUMP_HZ;
        else
            new_frequency = prev_frequency - RATE_LIMIT_MAX_JUMP_HZ;
    }
    prev_frequency = new_frequency;

    // Setze globale Variablen: Die Hauptfrequenz wird als neuer, limitierter Wert ausgegeben
    main_frequency = new_frequency - OFFSET;
    max_magnitude = max_segment_sum;

    #if ENABLE_ADC_FFT_LOGS
        ESP_LOGI(TAG, "LF Main Frequency (window center, limited): %.2f Hz, Integrated Magnitude: %.2f",
                 main_frequency, max_magnitude);
    #endif

    free(magnitudes);

    // Speichere die Frequenzmessung – auch die Fastdetect-Chunks erhalten so diesen Wert.
    store_frequency(main_frequency);
}

void collect_adc_continuous_data() {
    ESP_ERROR_CHECK(adc_continuous_start(adc_handle));
    ESP_LOGI(TAG, "ADC started in continuous mode");

    size_t bytes_read = 0;
    while (1) {
        ESP_ERROR_CHECK(adc_continuous_read(adc_handle,
                                            (uint8_t *)adc_buffer,
                                            sizeof(adc_buffer),
                                            (uint32_t *)&bytes_read,
                                            portMAX_DELAY));
        if (bytes_read > 0) {
            #if ENABLE_ADC_FFT_LOGS
                ESP_LOGI(TAG, "Collected %d bytes of ADC data", bytes_read);
            #endif

            // Speichere die ADC-Daten im Ringpuffer
            memcpy(collected_data[current_buffer_index], adc_buffer, sizeof(adc_buffer));
            current_buffer_index = (current_buffer_index + 1) % NUM_BUFFERS;

            // FFT ausführen und Hauptfrequenz bestimmen
            perform_fft();
        }
        vTaskDelay(pdMS_TO_TICKS(ADC_TASK_DELAY_MS));
    }
}

void save_adc_data() {
    ESP_LOGI("ADC_FFT", "Saving ADC buffers...");

    if (adc_semaphore == NULL) {
        adc_semaphore = xSemaphoreCreateBinary();
        if (adc_semaphore == NULL) {
            ESP_LOGE("ADC_FFT", "Failed to create ADC semaphore");
            return;
        }
        xSemaphoreGive(adc_semaphore);
    }

    if (xSemaphoreTake(adc_semaphore, pdMS_TO_TICKS(1000)) == pdTRUE) {
        for (int i = 0; i < NUM_BUFFERS; i++) {
            size_t bytes_read = 0;
            ESP_ERROR_CHECK(adc_continuous_read(adc_handle,
                                                (uint8_t *)collected_data[i],
                                                sizeof(collected_data[i]),
                                                (uint32_t *)&bytes_read,
                                                portMAX_DELAY));
            ESP_LOGI("ADC_FFT", "Buffer %d/%d saved", i + 1, NUM_BUFFERS);
        }
        xSemaphoreGive(adc_semaphore);
        ESP_LOGI("ADC_FFT", "All buffers saved.");
    } else {
        ESP_LOGE("ADC_FFT", "Failed to acquire ADC semaphore");
    }
}
