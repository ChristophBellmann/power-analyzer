pandoc --from markdown --template eisvogel --filter pandoc-latex-environment --filter pandoc-include --listing --citeproc -o ./../article.pdf article.md
