#ifndef CONFIG_H
#define CONFIG_H

// ADC/FFT
#define SAMPLE_RATE       200000
#define FFT_SIZE          1024
#define ADC_TASK_DELAY_MS 100

// WebSocket-Puffer
#define BUF_LEN FFT_SIZE

// Fastdetect
#define NUM_CHUNKS       20
#define JSON_BUFFER_SIZE 1024

// Wi-Fi
#define WIFI_SSID "Pixel_cb"
#define WIFI_PASS "Lassmichrein!"

#endif // CONFIG_H
