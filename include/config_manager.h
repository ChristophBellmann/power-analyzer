#pragma once
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Lädt `/spiffs/config.yaml` oder verwendet Defaults. */
esp_err_t config_manager_load(void);

/** Speichert aktuelle Einstellungen nach `/spiffs/config.yaml`. */
esp_err_t config_manager_save(void);

// Getter
int         config_get_trace_width(void);
int         config_get_grid_width(void);
const char* config_get_trace_v_color(void);
const char* config_get_trace_i_color(void);

// Setter
void config_set_trace_width(int w);
void config_set_grid_width(int w);
void config_set_trace_v_color(const char *hex);
void config_set_trace_i_color(const char *hex);

#ifdef __cplusplus
}
#endif
