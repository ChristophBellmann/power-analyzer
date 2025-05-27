idf_component_register(
    SRCS
        adc.c fft.c harmonics.c recorder.c spiffs_init.c webserver.c wifi.c main.c config_manager.c
    INCLUDE_DIRS
        "../include"          # public
    PRIV_INCLUDE_DIRS
        "."                   # private header direkt in main/, falls nötig
)
