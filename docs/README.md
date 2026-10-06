# ESP32 Measurement & Spectrum Analysis

A family of ESP32 measurement projects for real-time waveform acquisition, frequency-domain analysis and browser-based visualization.

The documentation combines two closely related development stages:

- **Spectrum Analyzer** — the earlier real-time audio/signal-analysis platform, using continuous ADC acquisition, FFT processing, WebSocket data and browser visualizations.
- **Power Analyzer** — the later electrical measurement platform focused on voltage/current waveforms, harmonic content, THD and downloadable measurement data.

Keeping both stages together makes the technical evolution visible without presenting two strongly overlapping systems as unrelated portfolio projects.

![Oscilloscope view](../doc/02-pictures/Scope-Screenshot-22052025.png)

## Current Power Analyzer

- Live voltage and current oscilloscope in the browser
- Harmonic analysis including DC component, harmonics and THD
- FFT processing on the ESP32
- Binary measurement recorder for offline analysis
- HTTP and WebSocket interfaces
- SPIFFS-hosted frontend
- Python-friendly recorded data format

## Documentation

Start with [System overview](overview.md). The [Spectrum Analyzer](spectrum-analyzer.md) chapter documents the earlier project stage and its relationship to the Power Analyzer. The remaining chapters describe the current electrical measurement platform.

The repository also contains the original project documentation and application article; see [Technical documents](technical-documents.md).

---

**Developed by Renewable Energy Design**  
Engineering · Embedded Systems · Measurement · Energy · Automation
