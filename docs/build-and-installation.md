# Build & installation

## Requirements

Both applications are verified with ESP-IDF 5.4.0. Install and activate that environment before building. PlatformIO configurations pin Espressif32 6.10.0, which supplies ESP-IDF 5.4.0.

## Build

First initialize the shared ESP-DSP dependency:

```bash
git clone --recurse-submodules https://github.com/ChristophBellmann/power-analyzer.git
cd power-analyzer
# For an existing clone:
git submodule update --init --recursive
```

From the repository root (electrical Power Analyzer):

```bash
idf.py build
```

## Flash firmware

```bash
idf.py -p /dev/ttyUSB0 flash
```

Replace the serial device with the port used by your ESP32.

## Monitor

```bash
idf.py -p /dev/ttyUSB0 monitor
```

## Flash the SPIFFS frontend

```bash
idf.py spiffs-flash
```

The repository also contains PlatformIO configuration. The ESP-IDF project files remain the primary reference for the current firmware configuration.

## After flashing

Connect the device to the configured network and open its IP address in a browser to access the web interface.

## Spectrum Analyzer variant

This is a separate application. With ESP-IDF 5.4 activated:

```bash
cd firmware/spectrum-analyzer
idf.py set-target esp32
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

Configure Wi-Fi locally in `include/config.h` within that directory before flashing.
The build uses the shared root ESP-DSP submodule and creates its SPIFFS image from
`../../data/spectrum-analyzer`. `flash` includes this image; `spiffs-flash` updates
only the frontend. PlatformIO is also supported from this directory with `pio run`
and `pio run -t uploadfs`; the platform is pinned to Espressif32 6.10.0.

Flashing a variant replaces the firmware and frontend on the device. The two
variants have separate HTTP/WebSocket protocols. See the
[firmware README](https://github.com/ChristophBellmann/power-analyzer/tree/main/firmware/spectrum-analyzer)
for endpoint and hardware details.

Each firmware packages only its own frontend directory. The electrical variant uses
`data/power-analyzer`; the Spectrum variant uses `data/spectrum-analyzer`. The
electrical firmware's diagnostic PSRAM size query is conditional on `CONFIG_SPIRAM`,
so builds without PSRAM support also link successfully.
