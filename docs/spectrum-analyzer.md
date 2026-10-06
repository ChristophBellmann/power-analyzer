# Spectrum Analyzer

The **ESP32 Spectrum Analyzer** is the signal-analysis branch of this repository. It focuses on real-time audio/frequency analysis and is maintained alongside the electrical Power Analyzer as part of the same embedded measurement project family.

## Integrated project structure

The Spectrum Analyzer material is organized by function rather than kept as a legacy dump:

- `firmware/spectrum-analyzer/` — ESP32 firmware variant and build configuration
- `data/spectrum-analyzer/` — browser interfaces for live monitoring and fast detection
- `analysis/spectrum-analyzer/` — FFT/audio analysis notebooks and supporting calculations
- `hardware/cad/spectrum-analyzer/` — enclosure and microphone CAD assets
- `docs/assets/spectrum-analyzer/` — interface and hardware reference images

The Power Analyzer remains the current electrical-measurement implementation under the repository's normal `main/`, `include/` and `data/` structure.

## Processing chain

```text
ESP32 ADC
→ continuous high-speed acquisition
→ smoothing / normalization
→ FFT
→ dominant-frequency and trend analysis
→ WebSocket
→ browser visualization
```

The Spectrum Analyzer uses a 1024-sample FFT workflow and browser interfaces for spectrum/history monitoring, frequency detection and signal inspection.

## Relationship to the Power Analyzer

Both variants share the same engineering pattern: acquire analogue signals continuously on an ESP32, process them locally, and expose results through a web interface.

The Power Analyzer extends this architecture toward voltage/current waveforms, harmonic analysis, total harmonic distortion and measurement recording. Keeping both variants in one repository preserves the development path while avoiding two overlapping portfolio projects.

## Dependency handling

Third-party ESP-DSP sources are not duplicated specifically for the Spectrum Analyzer. The repository already contains the ESP-DSP component used by the project family.
