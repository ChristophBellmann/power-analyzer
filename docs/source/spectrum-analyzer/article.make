#!/bin/sh
set -eu
cd "$(dirname "$0")"
mkdir -p ../../../build/spectrum-docs
pandoc --from markdown --template ./eisvogel.latex --filter pandoc-latex-environment --filter pandoc-include --listings --citeproc -o ../../../build/spectrum-docs/article.pdf article.md
