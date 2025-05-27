#include "config_manager.h"
#include "config.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "CONFIG_MGR";

// interne Speicherplätze, initialisiert mit den UI_DEFAULTS
static int   s_trace_width_px = UI_TRACE_WIDTH_PX;
static int   s_grid_width_px  = UI_GRID_LINE_WIDTH_PX;
static char  s_trace_v_color[16] = UI_TRACE_COLOR_VOLTAGE;
static char  s_trace_i_color[16] = UI_TRACE_COLOR_CURRENT;

esp_err_t config_manager_load(void)
{
    FILE *f = fopen("/spiffs/config.yaml", "r");
    if (!f) {
        ESP_LOGW(TAG, "Keine config.yaml, nutze Defaults");
        return ESP_OK;
    }
    // In diesem Beispiel ignorieren wir den Inhalt und behalten die Defaults.
    // Hier könnten Sie eine echte YAML-Bibliothek einbauen.
    fclose(f);
    return ESP_OK;
}

esp_err_t config_manager_save(void)
{
    FILE *f = fopen("/spiffs/config.yaml", "w");
    if (!f) {
        ESP_LOGE(TAG, "config.yaml kann nicht geschrieben werden");
        return ESP_FAIL;
    }
    // Sehr einfache YAML-Ausgabe
    fprintf(f,
        "traceWidth: %d\n"
        "gridWidth: %d\n"
        "traceVColor: \"%s\"\n"
        "traceIColor: \"%s\"\n",
        s_trace_width_px,
        s_grid_width_px,
        s_trace_v_color,
        s_trace_i_color
    );
    fclose(f);
    ESP_LOGI(TAG, "config.yaml gespeichert");
    return ESP_OK;
}

// Getter
int         config_get_trace_width(void)  { return s_trace_width_px; }
int         config_get_grid_width(void)   { return s_grid_width_px; }
const char* config_get_trace_v_color(void) { return s_trace_v_color; }
const char* config_get_trace_i_color(void) { return s_trace_i_color; }
// Setter
void config_set_trace_width(int w)   { s_trace_width_px = w; }
void config_set_grid_width(int w)    { s_grid_width_px  = w; }
void config_set_trace_v_color(const char *hex)
{
    strncpy(s_trace_v_color, hex, sizeof(s_trace_v_color)-1);
    s_trace_v_color[sizeof(s_trace_v_color)-1] = '\0';
}
void config_set_trace_i_color(const char *hex)
{
    strncpy(s_trace_i_color, hex, sizeof(s_trace_i_color)-1);
    s_trace_i_color[sizeof(s_trace_i_color)-1] = '\0';
}
