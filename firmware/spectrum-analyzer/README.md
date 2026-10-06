# Spectrum Analyzer firmware

This is the audio/frequency firmware variant of [Power Analyzer](../../README.md).
It is a separate ESP-IDF application sharing the pinned ESP-DSP component at
`../../components/esp-dsp`. Its frontend is built from `../../data/spectrum-analyzer`.

## Build and flash

Initialize the shared dependency from the repository root:

```sh
git submodule update --init --recursive
```

With ESP-IDF 5.4 activated, run from this directory:

```sh
idf.py set-target esp32
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

The SPIFFS image is included in `flash`; to update only the frontend:

```sh
idf.py -p /dev/ttyUSB0 spiffs-flash
```

Or use PlatformIO (Espressif32 6.10.0 / ESP-IDF 5.4):

```sh
pio run
pio run -t upload
pio run -t uploadfs
pio device monitor
```

Set `WIFI_SSID` and `WIFI_PASS` in `include/config.h` locally before flashing.
Do not commit personal credentials. ADC channel 0 corresponds to GPIO36 on ESP32;
input must remain within the ESP32 ADC's electrical limits.

Open `/`, `/fastdetect`, or `/monitoring` on the device. This firmware provides
`/ws` and `/wav`; the electrical firmware has a different API. Flashing one
variant replaces the other. No simultaneous operation is implemented.

The source configures 44,100 samples/s and a 1024-point FFT (about 43.1 Hz/bin,
22.05 kHz Nyquist limit). Actual capture timing and detection accuracy require
measurement on hardware. See [analysis](../../analysis/spectrum-analyzer/README.md)
and the [migration record](../../docs/spectrum-analyzer-migration.md).
