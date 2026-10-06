#ifndef ADC_FFT_H
#define ADC_FFT_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_adc/adc_continuous.h"
#include "config.h"  // Enthält u.a. NUM_BUFFERS, FFT_SIZE, SAMPLE_RATE, LF_LOW_FREQ, LF_HIGH_FREQ, etc.

// ADC-Handle für den kontinuierlichen Betrieb
extern adc_continuous_handle_t adc_handle;

// Globale Variablen für die ermittelte Hauptfrequenz (LF-Bereich) und deren Magnitude
extern float main_frequency;
extern float max_magnitude;

// Ringpuffer für die FFT-Daten
extern int16_t collected_data[NUM_BUFFERS][FFT_SIZE];
extern int current_buffer_index; // Aktueller Index im Ringpuffer

// Initialisiert den ADC im Continuous-Modus
void configure_adc_continuous();

// Startet den Task, der ADC-Daten sammelt und die FFT ausführt
void collect_adc_continuous_data();

// Führt die FFT aus, bestimmt die Hauptfrequenz im LF-Bereich anhand eines gleitenden Fensters,
// berücksichtigt die relative Amplitude und wendet Rate Limiting an.
void perform_fft();

// Speichert die ADC-Daten (Beispielimplementierung)
void save_adc_data();

#endif // ADC_FFT_H