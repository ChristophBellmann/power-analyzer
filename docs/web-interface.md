# Web interface

The ESP32 hosts its own browser interface.

Typical access:

```text
http://<ESP-IP>/
```

## Oscilloscope

The oscilloscope presents live voltage and current waveforms.

![Oscilloscope](../doc/02-pictures/Scope-Screenshot-22052025.png)

## Harmonic analysis

A separate view presents spectral/harmonic information and THD.

![Harmonics](../doc/02-pictures/Harmonics-Screenshot-22052025.png)

## Interfaces

The project includes HTTP and WebSocket functionality for live data and recorded-data access. Endpoint names and payloads should be checked against the current `main/webserver.c` implementation when integrating external software.
