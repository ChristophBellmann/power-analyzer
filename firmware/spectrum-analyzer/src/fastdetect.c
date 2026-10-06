#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/param.h>
#include "esp_log.h"
#include "config.h"
#include "fastdetect.h"

static const char *TAG = "FASTDETECT";

#define FREQ_STORAGE_SIZE 64

static float s_freqStorage[FREQ_STORAGE_SIZE];
static int s_freqWritePos = 0;
static int s_freqCount = 0;

/* Speichert eine Frequenzmessung im ringförmigen Puffer. */
void store_frequency(float freq)
{
    s_freqStorage[s_freqWritePos] = freq;
    s_freqWritePos = (s_freqWritePos + 1) % FREQ_STORAGE_SIZE;
    if (s_freqCount < FREQ_STORAGE_SIZE)
    {
        s_freqCount++;
    }
}

/* Gibt die i-te zuletzt gespeicherte Frequenz zurück. */
static float get_recent_freq(int i)
{
    if (i < 0 || i >= s_freqCount)
    {
        return 0.0f;
    }
    int idx = s_freqWritePos - 1 - i;
    if (idx < 0)
    {
        idx += FREQ_STORAGE_SIZE;
    }
    return s_freqStorage[idx];
}

/* Ringpuffer für die Trendanalyse (Chunks). */
static float s_chunkFreq[NUM_CHUNKS];
static char s_chunkTrend[NUM_CHUNKS][8]; // "rise", "fall" oder "same"

/* Vergleicht den neuen Wert mit dem alten und gibt den Trend zurück. */
static const char* get_trend_str(float newVal, float oldVal)
{
    float diff = newVal - oldVal;
    if (fabsf(diff) < 0.001f)
    {
        return "same";
    }
    return (diff > 0) ? "rise" : "fall";
}

/**
 * Baut einen JSON-String, der die gespeicherten Chunks enthält.
 * Ausgabeformat: {"chunks":[{"freq":<Wert>,"trend":"<Wert>"}, ...]}
 */
void build_chunk_json(char *outbuf, size_t outsize)
{
    int written = snprintf(outbuf, outsize, "{\"chunks\":[");
    if (written < 0 || written >= outsize)
    {
        ESP_LOGE(TAG, "JSON buffer zu klein am Anfang.");
        return;
    }
    size_t offset = written;
    for (int i = 0; i < NUM_CHUNKS; i++)
    {
        float freq = s_chunkFreq[i];
        const char *trend = s_chunkTrend[i];
        char entry[64];
        int entry_len = snprintf(entry, sizeof(entry),
                                 "{\"freq\":%.2f,\"trend\":\"%s\"}%s",
                                 freq, trend, (i < NUM_CHUNKS - 1) ? "," : "");
        if (entry_len < 0 || (offset + entry_len) >= outsize - 2)
        {
            ESP_LOGE(TAG, "JSON buffer zu klein beim Hinzufügen der Chunks.");
            break;
        }
        strcat(outbuf, entry);
        offset += entry_len;
    }
    if ((offset + 2) < outsize)
    {
        strcat(outbuf, "]}");
    }
    else
    {
        ESP_LOGE(TAG, "JSON buffer zu klein zum Schließen des JSON.");
    }
}

/**
 * Fastdetect Task:
 * - Alle 100 ms werden die neuesten 3 Frequenzmessungen verarbeitet.
 * - Wenn FASTDETECT_ENABLE_PARABOLIC_INTERP aktiviert ist, wird eine parabolische Interpolation durchgeführt,
 *   ansonsten wird der Mittelwert der 3 Messungen genommen.
 * - Der Chunk-Ring wird verschoben, und der neue Chunk wird an Index 0 abgelegt.
 */
static void fast_detect_task(void *arg)
{
    int i;
    /* Initialisiere den Chunk-Ring */
    for (i = 0; i < NUM_CHUNKS; i++)
    {
        s_chunkFreq[i] = 0.0f;
        strcpy(s_chunkTrend[i], "same");
    }
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
        
        float measurements[FASTDETECT_NUM_MEASUREMENTS];
        int count = 0;
        for (i = 0; i < FASTDETECT_NUM_MEASUREMENTS; i++)
        {
            float f = get_recent_freq(i);
            measurements[i] = f;
            if (f != 0.0f)
            {
                count++;
            }
        }
        if (count < FASTDETECT_NUM_MEASUREMENTS)
        {
            ESP_LOGI(TAG, "Nicht genügend Frequenzdaten, Chunk-Aktualisierung übersprungen...");
            continue;
        }
        
        float refined = 0.0f;
        #if FASTDETECT_ENABLE_PARABOLIC_INTERP
            // Parabolische Interpolation:
            // Annahme: measurements[2] = f(-1), measurements[1] = f(0), measurements[0] = f(1)
            float f_left  = measurements[2];
            float f_mid   = measurements[1];
            float f_right = measurements[0];
            float denom = f_left - 2.0f * f_mid + f_right;
            float d = 0.0f;
            if (fabs(denom) > 1e-6)
            {
                d = 0.5f * (f_left - f_right) / denom;
            }
            refined = f_mid + d;
        #else
            // Fallback: Mittelwertbildung
            float sum = 0.0f;
            for (i = 0; i < FASTDETECT_NUM_MEASUREMENTS; i++)
            {
                sum += measurements[i];
            }
            refined = sum / FASTDETECT_NUM_MEASUREMENTS;
        #endif

        /* Verschiebe den Chunk-Ring: Ältere Chunks rutschen weiter */
        for (i = NUM_CHUNKS - 1; i > 0; i--)
        {
            s_chunkFreq[i] = s_chunkFreq[i - 1];
            strcpy(s_chunkTrend[i], s_chunkTrend[i - 1]);
        }
        float oldVal = s_chunkFreq[1];
        s_chunkFreq[0] = refined;
        const char* trend = get_trend_str(refined, oldVal);
        strcpy(s_chunkTrend[0], trend);
        ESP_LOGI(TAG, "Chunk=%.2f => %s vs %.2f", refined, trend, oldVal);
    }
}

/**
 * Initialisiert die Fastdetect-Task.
 */
void init_fastdetect_task(void)
{
    xTaskCreate(fast_detect_task, "fast_detect_task", 4096, NULL, 5, NULL);
}
