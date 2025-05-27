#pragma once
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/*  Initialisiert WLAN + Scan/Connect/Hotspot-Logik */
esp_err_t wifi_init(void);

/*  Optionaler manueller Re-Scan */
void wifi_start_scan(void);

#ifdef __cplusplus
}
#endif
