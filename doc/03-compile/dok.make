pandoc --quiet --from markdown --template eisvogel --filter pandoc-latex-environment --filter pandoc-include --listing --citeproc -o ./../Dokumentation_Power-Analyzer.pdf dok.md
