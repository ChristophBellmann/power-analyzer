# ESP32 Power Analyzer

An ESP32-based measurement platform for observing voltage and current waveforms, recording measurement data, and analysing harmonic content and total harmonic distortion (THD).

The project combines an isolated analogue front end, continuous ADC acquisition, FFT-based signal processing, an embedded web interface and downloadable measurement data.

![Oscilloscope view](../doc/02-pictures/Scope-Screenshot-22052025.png)

## What it does

- Live voltage and current oscilloscope in the browser
- Harmonic analysis including DC component, harmonics and THD
- FFT processing on the ESP32
- Binary measurement recorder for offline analysis
- HTTP and WebSocket interfaces
- SPIFFS-hosted frontend
- Python-friendly recorded data format

## Documentation

Start with [System overview](overview.md), then continue with [Hardware](hardware.md), [Measurement & FFT](measurement-and-fft.md), [Software architecture](software-architecture.md), [Web interface](web-interface.md), [Data recording](data-recording.md), and [Build & installation](build-and-installation.md).

The repository also contains the original project documentation and application article; see [Technical documents](technical-documents.md).

---

**Developed by Renewable Energy Design**  
Engineering · Embedded Systems · Energy · Automation
