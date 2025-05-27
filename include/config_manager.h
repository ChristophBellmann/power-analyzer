// include/config_manager.h
#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Pfad zur Konfigurationsdatei auf SPIFFS */
#define CONFIG_FILE_PATH   "/spiffs/config.yaml"

/** Getter / Setter für Anzeige-Parameter */
uint16_t config_get_trace_width(void);
void     config_set_trace_width(uint16_t w);

uint16_t config_get_grid_width(void);
void     config_set_grid_width(uint16_t w);

const char* config_get_trace_v_color(void);
void        config_set_trace_v_color(const char* c);

const char* config_get_trace_i_color(void);
void        config_set_trace_i_color(const char* c);

/** Theme 1 oder 2 */
int  config_get_theme(void);
void config_set_theme(int theme);

/** Dateien laden / speichern */
esp_err_t config_manager_load(void);
esp_err_t config_manager_save(void);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_MANAGER_H
