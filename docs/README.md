# ESP32 Measurement & Spectrum Analysis

A family of ESP32 measurement projects for real-time waveform acquisition, frequency-domain analysis and browser-based visualization.

The documentation covers two separately buildable firmware variants in one maintained repository:

- **Spectrum Analyzer** — the earlier real-time audio/signal-analysis platform, using continuous ADC acquisition, FFT processing, WebSocket data and browser visualizations.
- **Power Analyzer** — the later electrical measurement platform focused on voltage/current waveforms, harmonic content, THD and downloadable measurement data.

Both variants are maintained in
[ChristophBellmann/power-analyzer](https://github.com/ChristophBellmann/power-analyzer).
SpectrumAnalyzer is fully consolidated: firmware, all three browser UIs, both
analysis notebooks, printable enclosure models, pictures, original documents and
attribution are preserved. The predecessor repository is ready for archival.

## Choose a firmware variant

| Variant | Firmware | Frontend | Purpose |
| --- | --- | --- | --- |
| Power Analyzer | Repository root | `data/power-analyzer/` | Voltage/current waveforms, harmonics, THD and recording |
| Spectrum Analyzer | `firmware/spectrum-analyzer/` | `data/spectrum-analyzer/` | Audio/frequency analysis, trends and WAV inspection |

Each application has its own firmware and SPIFFS image. Both use the same pinned
ESP-DSP fork. See [Build & installation](build-and-installation.md) for cloning,
initializing the submodule, building and flashing the chosen variant.

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

Start with [System overview](overview.md). The [Spectrum Analyzer](spectrum-analyzer.md) chapter explains the audio/frequency variant, its browser interfaces and its relationship to the Power Analyzer. The remaining chapters describe the current electrical measurement platform.

- [Spectrum Analyzer](spectrum-analyzer.md): processing chain and browser screenshots.
- [Hardware](hardware.md): electrical sensing, STL enclosures and CAD attribution.
- [Technical documents](technical-documents.md): original PDFs and regeneration workflow.
- [Spectrum Analyzer migration](spectrum-analyzer-migration.md): complete file inventory, shared dependency and verification results.

Both firmware builds, their separate SPIFFS images and both Spectrum document
regeneration workflows were verified on 2026-10-06. Hardware runtime measurements
remain a separate validation step.

---

**Developed by Renewable Energy Design**  
Engineering · Embedded Systems · Measurement · Energy · Automation
