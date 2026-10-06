# System overview

The ESP32 Power Analyzer is intended as a compact network-connected measurement platform for observing electrical waveforms and analysing harmonic content.

## Measurement chain

The system follows this path:

1. Isolated voltage and current sensing
2. Analogue signal conditioning
3. Continuous ESP32 ADC acquisition
4. DMA-backed block processing
5. FFT and harmonic analysis
6. Live presentation through HTTP/WebSocket
7. Optional binary recording for offline analysis

The current firmware exposes oscilloscope, harmonic-analysis and recorder functionality through its embedded web server.

## Design goals

The project explores a low-cost embedded approach to remote measurement and signal analysis. It is particularly useful as an engineering platform for experimenting with ADC acquisition, FFT processing, harmonic analysis and browser-based visualization.

> **Safety:** Work on mains-connected measurement hardware requires appropriate isolation, protection and electrical competence. The project documentation is not a substitute for a safety assessment or certified measurement equipment.
