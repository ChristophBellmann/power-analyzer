# Software architecture

The firmware separates acquisition/signal processing, recording and network presentation into dedicated modules.

Key source areas include:

- `main/` — application initialization and firmware modules
- `include/` — public headers and configuration
- `data/power-analyzer/` — electrical SPIFFS web frontend
- `data/spectrum-analyzer/` — audio/frequency SPIFFS web frontend
- `components/` — project components
- `doc/` — original technical documentation and figures

## Main responsibilities

### ADC and FFT

Acquires measurement samples and performs FFT-based processing.

### Recorder

Provides captured voltage/current data for download and offline analysis.

### Web server

Hosts the browser interface and exposes HTTP/WebSocket endpoints.

### Frontend

The SPIFFS-hosted pages provide the oscilloscope and harmonic-analysis views.

For implementation details, use the current source tree as the authoritative reference because the firmware can evolve faster than this overview.

## Repository boundary

The two firmware variants and their pinned ESP-DSP fork form one independently
maintained project. Neither a Thinkthing checkout nor the REDesign website build
is needed to clone or build it. Initialize `components/esp-dsp` using
`git submodule update --init --recursive` before building. The external
superproject reference was removed on 2026-10-15; the DSP dependency was retained.
