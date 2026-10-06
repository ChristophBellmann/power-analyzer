# Spectrum Analyzer

The **ESP32 Spectrum Analyzer** is an earlier, closely related project stage that focuses on real-time signal and audio-frequency analysis.

Its original implementation is retained in the separate `SpectrumAnalyzer` repository for development history, while this GitBook is the canonical documentation location for the combined measurement-project family.

## Processing chain

The Spectrum Analyzer follows this embedded pipeline:

```text
ESP32 ADC
→ continuous high-speed acquisition
→ smoothing / normalization
→ FFT
→ dominant-frequency and trend analysis
→ WebSocket
→ browser visualization
```

The original project documents a 1024-sample acquisition/FFT workflow and browser interfaces for spectrum/history monitoring, an RPM-style display and short WAV sample playback.

## Relationship to the Power Analyzer

Both projects share the same core engineering pattern: acquire analogue signals continuously on an ESP32, process them locally, and expose the results through a web interface.

The later Power Analyzer applies that architecture to electrical measurements and extends the focus toward:

- voltage and current waveforms,
- harmonic analysis,
- total harmonic distortion,
- measurement recording,
- a more structured electrical measurement workflow.

For the portfolio, the Spectrum Analyzer is therefore treated as a predecessor and signal-processing branch of the Power Analyzer rather than as a second standalone product.

## Source history

The original implementation remains available in `ChristophBellmann/SpectrumAnalyzer`. Keeping that repository preserves code and commit history; new portfolio documentation should be maintained here in the Power Analyzer GitBook.
