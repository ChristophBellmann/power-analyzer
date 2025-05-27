// main/config_manager.c
#include "config_manager.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "CONFIG_MGR";

/* interne State-Variablen mit Default-Werten */
static uint16_t s_trace_width_px   = 2;
static uint16_t s_grid_width_px    = 1;
static char     s_trace_v_color[16] = "#ffeb3b";
static char     s_trace_i_color[16] = "#00bcd4";
static int      s_theme            = 1;

uint16_t config_get_trace_width(void)   { return s_trace_width_px; }
void     config_set_trace_width(uint16_t w) { s_trace_width_px = w; }

uint16_t config_get_grid_width(void)    { return s_grid_width_px; }
void     config_set_grid_width(uint16_t w)  { s_grid_width_px = w; }

const char* config_get_trace_v_color(void) { return s_trace_v_color; }
void        config_set_trace_v_color(const char* c) {
    strncpy(s_trace_v_color, c, sizeof(s_trace_v_color)-1);
    s_trace_v_color[sizeof(s_trace_v_color)-1] = '\0';
}

const char* config_get_trace_i_color(void) { return s_trace_i_color; }
void        config_set_trace_i_color(const char* c) {
    strncpy(s_trace_i_color, c, sizeof(s_trace_i_color)-1);
    s_trace_i_color[sizeof(s_trace_i_color)-1] = '\0';
}

int config_get_theme(void)        { return s_theme; }
void config_set_theme(int theme)  { s_theme = theme; }

/**
 *  Lädt die config.yaml, falls vorhanden.
 *  Zeilen werden per sscanf geparst.
 */
esp_err_t config_manager_load(void)
{
    FILE *f = fopen(CONFIG_FILE_PATH, "r");
    if (!f) {
        ESP_LOGW(TAG, "%s nicht gefunden, verwende Default-Werte", CONFIG_FILE_PATH);
        return ESP_FAIL;
    }
    char line[128];
    while (fgets(line, sizeof(line), f)) {
        int iv;
        if (sscanf(line, "traceWidth: %d", &iv) == 1) {
            s_trace_width_px = iv;
        } else if (sscanf(line, "gridWidth: %d", &iv) == 1) {
            s_grid_width_px = iv;
        } else if (sscanf(line, "traceVColor: \"%15[^\"]\"", line) == 1) {
            strncpy(s_trace_v_color, line, sizeof(s_trace_v_color)-1);
        } else if (sscanf(line, "traceIColor: \"%15[^\"]\"", line) == 1) {
            strncpy(s_trace_i_color, line, sizeof(s_trace_i_color)-1);
        } else if (sscanf(line, "theme: %d", &iv) == 1) {
            s_theme = iv;
        }
    }
    fclose(f);
    ESP_LOGI(TAG,
        "Geladen: traceWidth=%d, gridWidth=%d, traceVColor=%s, traceIColor=%s, theme=%d",
        s_trace_width_px,
        s_grid_width_px,
        s_trace_v_color,
        s_trace_i_color,
        s_theme
    );
    return ESP_OK;
}

/**
 *  Speichert alle aktuellen Settings als YAML
 *  und loggt danach jede Zeile.
 */
esp_err_t config_manager_save(void)
{
    FILE *f = fopen(CONFIG_FILE_PATH, "w");
    if (!f) {
        ESP_LOGE(TAG, "%s kann nicht geschrieben werden", CONFIG_FILE_PATH);
        return ESP_FAIL;
    }
    fprintf(f,
        "traceWidth: %d\n"
        "gridWidth: %d\n"
        "traceVColor: \"%s\"\n"
        "traceIColor: \"%s\"\n"
        "theme: %d\n",
        s_trace_width_px,
        s_grid_width_px,
        s_trace_v_color,
        s_trace_i_color,
        s_theme
    );
    fclose(f);

    ESP_LOGI(TAG, "CONFIG_MGR: %s gespeichert, Inhalt:", CONFIG_FILE_PATH);
    FILE *r = fopen(CONFIG_FILE_PATH, "r");
    if (r) {
        char buf[128];
        while (fgets(buf, sizeof(buf), r)) {
            /* Zeilenumbruch entfernen */
            size_t l = strlen(buf);
            if (l && buf[l-1]=='\n') buf[l-1] = '\0';
            ESP_LOGI(TAG, "%s", buf);
        }
        fclose(r);
    } else {
        ESP_LOGW(TAG, "CONFIG_MGR: Konnte %s nicht zum Lesen öffnen", CONFIG_FILE_PATH);
    }

    return ESP_OK;
}
