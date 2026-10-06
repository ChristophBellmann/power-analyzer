# Build & installation

## Requirements

The project uses ESP-IDF. Install and activate a compatible ESP-IDF environment before building.

## Build

From the project directory:

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
