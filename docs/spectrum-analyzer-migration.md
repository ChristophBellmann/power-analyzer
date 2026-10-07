# Spectrum Analyzer migration

`ChristophBellmann/power-analyzer` is the canonical repository for both ESP32
measurement applications. `SpectrumAnalyzer` is archived on GitHub and retains its Git history as a
read-only predecessor. Further changes belong in `power-analyzer`.

## Audited source

The audit covers SpectrumAnalyzer commit
`59f32f2` (full commit and SHA-256 hashes are recorded in
[source/spectrum-analyzer/migration-manifest.json](source/spectrum-analyzer/migration-manifest.json)).
All 601 tracked source files are accounted for:

- 63 files preserved byte for byte, including all seven STLs, all enclosure/UI
  images, both notebooks, historical text, reference PDFs and CAD attribution.
- 12 adapted files: firmware build configuration, sanitized Wi-Fi configuration,
  ignore rules and Pandoc paths.
- 512 ESP-DSP files provided by the one shared pinned dependency, byte for byte.
- 14 deliberate exclusions: empty placeholders/editor metadata, generic scaffold
  READMEs, generated sdkconfig snapshots and the generated dependency lock.

Generated historical PDFs are retained as reference outputs; new generated
outputs go to ignored build directories. The PDF logo is a Pandoc source asset,
not an expendable generated output. No unique ZIP is tracked in the audited
SpectrumAnalyzer tree. Existing Power Analyzer PDF/ZIP artifacts are retained.

## Shared ESP-DSP

Both original copies have the same Git tree:
`3a5c2422ec4766382cb8f987f7eef0d86c0dc267`.

The shared submodule at `components/esp-dsp` uses the
[ChristophBellmann ESP-DSP fork](https://github.com/ChristophBellmann/esp-dsp/tree/power-analyzer-compatible).
It is pinned to the complete original snapshot, based on Espressif commit
`e2c4d115eb353ef9aa4d329c76f436e5d7aa7e52` (2024-11-04).
The inherited CMake change excludes `dsps_cplx_gen.S` from the source list;
no DSP source file was changed. Upstream `.github` automation is absent from the
original snapshot and remains absent from the compatibility branch.

Both applications discover this component before ESP-IDF configures the project.
A clone requires `--recurse-submodules`, or `git submodule update --init --recursive`.
Updates must explicitly change the pinned commit after verifying both variants.

## Integration and verification

The Spectrum application has explicit CMake sources, header paths and component
requirements. It builds a SPIFFS image from the three canonical Spectrum web UIs.
PlatformIO uses the same frontend directory and pins Espressif32 to 6.10.0.
Pandoc includes, CSL, local template, logo and output paths use the canonical
layout. The preserved original README is historical and retains its original
links and numerical claims; the maintained documentation supersedes them.

Recheck the migration from the repository root:

```sh
python3 scripts/verify_spectrum_migration.py
# If the original checkout is available:
python3 scripts/verify_spectrum_migration.py --source ../SpectrumAnalyzer
```

This verifies file hashes and the dependency pin. Firmware compilation,
SPIFFS generation and document regeneration are additional checks. Flashing,
ADC timing, web interaction and frequency detection require a connected ESP32;
archival completeness alone does not establish measured hardware performance.

## Repository handover

The predecessor README links to the canonical project, firmware and GitBook.
The Overview masterplan should report archive readiness only for the completed,
verified migration. Archiving the GitHub repository is a separate administrative
step; this migration preserves history without rewriting the predecessor tree.

## Verification results (2026-10-06)

- `pio run -d firmware/spectrum-analyzer`: passed with ESP-IDF 5.4.0.
- `pio run -d firmware/spectrum-analyzer -t buildfs`: passed; exactly the three Spectrum UIs included.
- `pio run`: passed for the electrical variant; optional PSRAM diagnostic guarded when support is disabled.
- `pio run -t buildfs`: electrical frontend only (`index.html`, `harmonics.html`, `config.yaml`).
- Both historical documents regenerated through Pandoc and LaTeX in an isolated filter environment.
- Migration verification passed against the original source revision; all maintained local Markdown links resolve.

The electrical frontend now lives in `data/power-analyzer/`, separate from
`data/spectrum-analyzer/`. This prevents unrelated Spectrum files entering the
electrical SPIFFS image and exceeding SPIFFS filename limits. Board interaction
was not tested by these build checks. GitBook publication was checked separately
on 2026-10-07 against all 12 maintained source pages.
