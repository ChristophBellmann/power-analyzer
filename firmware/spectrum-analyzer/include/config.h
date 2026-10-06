#ifndef CONFIG_H
#define CONFIG_H

// ---------------------
// Logging Configuration
// ---------------------
#define ENABLE_FASTDETECT_LOGS 0  // Set to 1 to enable logs for fastdetect, 0 to disable
#define ENABLE_ADC_FFT_LOGS 0     // Set to 1 to enable logs for adc_fft, 0 to disable

// ---------------------
// Audio and FFT Configuration
// ---------------------
#define SAMPLE_RATE 44100          // Sample rate in Hz
#define FFT_SIZE 1024              // FFT size (number of samples to collect)
#define ADC_CHANNEL ADC_CHANNEL_0  // ADC channel
#define ADC_ATTEN ADC_ATTEN_DB_12  // ADC attenuation (0-3.3V range)

// ---------------------
// Frequency Range & Detection Parameters
// ---------------------
// LF-Bereich: nur Frequenzen zwischen LF_LOW_FREQ und LF_HIGH_FREQ werden untersucht.
#define LF_LOW_FREQ 200.0f         // untere Grenze des LF-Bereichs in Hz
#define LF_HIGH_FREQ 2500.0f       // obere Grenze des LF-Bereichs in Hz

// Fensterbreite für die Segmentintegration (z. B. 200 Hz)
#define WINDOW_BANDWIDTH_HZ 200.0f

// Rate Limiter: maximal erlaubter Frequenzsprung pro Messzyklus (z. B. alle 10 ms)
#define RATE_LIMIT_MAX_JUMP_HZ 200.0f

// Minimal erlaubte integrierte Amplitude im analysierten LF-Bereich.
// Liegt der Wert unter diesem Schwellwert, wird die Hauptfrequenz als 0 ausgegeben.
#define MIN_TOTAL_AMPLITUDE 1000.0f

// ---------------------
// Wi-Fi Configuration
// ---------------------
#define WIFI_SSID "Pixel_cb"        // Wi-Fi SSID
#define WIFI_PASS "Lassmichrein!"   // Wi-Fi Password

// ---------------------
// Buffer and Task Configuration
// ---------------------
#define NUM_BUFFERS 60             // Number of buffers in the ring buffer
#define ADC_TASK_DELAY_MS 10       // ADC task delay in milliseconds (z. B. 10 ms für schnellere Aktualisierung)

// ---------------------
// Frequency and JSON Configuration
// ---------------------
#define FREQ_STORAGE_SIZE 64       // Size of the frequency storage buffer
#define NUM_CHUNKS 20              // Number of chunks for trend analysis
#define JSON_BUFFER_SIZE 1024      // Buffer size for JSON data (1536)

// ---------------------
// Fastdetect Configuration
// ---------------------
#define FASTDETECT_NUM_MEASUREMENTS 3           // Anzahl der Messungen pro Chunk
#define FASTDETECT_ENABLE_PARABOLIC_INTERP 1      // 1 = Parabolische Interpolation aktivieren, 0 = nur Mittelwert

#define OFFSET 320

#endif // CONFIG_H
