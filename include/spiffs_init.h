#pragma once
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Mountet die Partition "spiffs" unter /spiffs.
 *  Gibt ESP_OK bei Erfolg zurück. */
esp_err_t spiffs_init(void);

#ifdef __cplusplus
}
#endif
