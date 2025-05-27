#ifndef CONFIG_H
#define CONFIG_H

/* ---------- ADC / FFT -------------------------------------------------- */
#define SAMPLE_RATE        200000           /* 200 kS/s                   */
#define FFT_SIZE           1024
#define NUM_BUFFERS        60
#define ADC_TASK_DELAY_MS  10               /* FFT-Task Zyklus (ms)       */

/* ---------- Analog-Skalierung (Download-Datei) ------------------------- */
#define VREF   3.300f                       /* Volt   pro ADC-Vollskala   */
#define IREF  20.000f                       /* Ampere pro ADC-Vollskala   */

/* ---------- Wi-Fi ------------------------------------------------------ */
#define WIFI_AP_CHANNEL          6
#define WIFI_AP_FALLBACK_SSID    "ESP32-Oszilloskop"
#define WIFI_AP_FALLBACK_PWD     ""          /* offen                      */

/* --- Bekannte Netze ( ►  wifi.c ) -------------------------------------- */
#define WIFI_CRED_CT             2           /* Anzahl Einträge unten      */

#define WIFI_SSID_1   "Ultranet"
#define WIFI_PASS_1   "Lassmichrein!"

#define WIFI_SSID_2   "Pixel_cb"
#define WIFI_PASS_2   "pixelpass"

#endif /* CONFIG_H */
