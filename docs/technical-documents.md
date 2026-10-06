# Technical documents

The repository contains the original project documentation in addition to this web-oriented GitBook documentation.

## Original documentation

- [Power Analyzer documentation (PDF)](../doc/Dokumentation_Power-Analyzer.pdf)
- [Power Analyzer application article (PDF)](../doc/Artikel_Power-Analyzer.pdf)

The source material used to produce the original documentation is stored under `doc/`.

These documents provide historical and project context. For current firmware behavior, always compare them with the current source code and repository README.

## Spectrum Analyzer historical documents

- [Original documentation PDF](archive/spectrum-analyzer/documentation.pdf)
- [Original article PDF](archive/spectrum-analyzer/article.pdf)

These reference outputs include historical template content. They are preserved
as original artifacts and do not establish verified current firmware behavior.
The corresponding text is under `docs/archive/spectrum-analyzer/`; the editable
Pandoc sources are under `docs/source/spectrum-analyzer/`.

Regenerate from the repository root:

```sh
sh docs/source/spectrum-analyzer/doc.make
sh docs/source/spectrum-analyzer/article.make
```

Requirements: Pandoc, a LaTeX installation with the packages used by the local
Eisvogel template, `pandoc-include`, and `pandoc-latex-environment`. Output goes to
`build/spectrum-docs/` and is ignored by Git. Include paths, citation style and the
source logo use their new repository locations.

A reproducible Python filter environment can be created separately:

```sh
python3 -m venv .venv-docs
. .venv-docs/bin/activate
pip install pandoc-include==1.4.4 pandoc-latex-environment==1.2.1.0
```
