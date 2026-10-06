# Measurement & FFT

## Continuous acquisition

The ESP32 continuous ADC path transfers samples into memory in blocks. DMA reduces CPU intervention during acquisition and allows the processor to handle signal processing and network tasks in parallel.

## FFT processing

The firmware performs FFT-based analysis to extract spectral information from the measured signal. The project is designed to report the DC component, harmonic amplitudes and total harmonic distortion (THD).

The implementation in the repository is the authoritative reference for current constants and processing details.

## Time window

For a sample rate of 200 kS/s and an FFT block of 1024 samples, one block represents:

```text
1024 / 200000 = 5.12 ms
```

This illustrates the relationship between acquisition rate, FFT size and the time span represented by each processing block.

## Harmonics

The user interface provides harmonic-analysis results alongside the live oscilloscope.

![Harmonic analysis](../doc/02-pictures/Harmonics-Screenshot-22052025.png)

When using harmonic results for compliance or engineering decisions, calibration, analogue bandwidth, ADC characteristics, windowing, spectral leakage and the applicable standard all need to be considered.
