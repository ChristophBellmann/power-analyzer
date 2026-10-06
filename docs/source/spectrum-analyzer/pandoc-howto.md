# Template
https://github.com/Wandmalfarbe/pandoc-latex-template/tree/master

# Sample
https://github.com/dpwiese/.pandoc/tree/main

# Erstellen der Gesamtdokumentation:

Führe die make-Datei aus dem Ordner 04-Erstellung aus:

```
~ project-sunnybot/doc/Erstellung$ ./Projekt2-Sunnybot-Dokumentation.make 

```

# Arbeiten mit pandoc an der Dokumentations-Datei:

Der folgende Einzeiler prüft, ob die Quelldatei gespeichert ist, und erstellt das PDF.

Dies ist sehr nützlich für den Arbeitsablauf beim Bearbeiten der .md-Datei, beim Speichern mit „Strg-S“ und beim automatischen Erhalten der .pdf-Ausgabe.

Das kann beim Schreiben einfach im Hintergrund laufen.

Im Verzeichnis wo die Dateien liegen ausführen.

```
while inotifywait -e close_write Kapitel/*; do pandoc --from markdown --template eisvogel --filter pandoc-latex-environment --filter pandoc-include --listing --citeproc -o GesamtDoku.pdf GesamtDoku.md; done

```

# Flussdiagramme

Diese können z.B. mit draw.io erstellt werden.
Wichtig ist, diese dann aber als PDF zu exportieren, um die SVG-Vektorisierung beizubehalten.
PNGs und JPGs sollten nur verwenden werden wenns nicht anders geht.

Wenn wer Lust hat, pandoc-mermaid aufzusetzen, zu testen und zu dokumentieren, gern. Habe das nicht zum laufen gebracht.

# markdown
http://svmiller.com/blog/2016/02/svm-r-markdown-manuscript/

# Verwendung einer schönen Pandoc-Vorlage:

[Open-University-Pandoc-Templates - die eisvogel.latex von hier wird verwendet](https://github.com/pfeifferj/Open-University-Pandoc-Templates)
[pandoc-latex-template - Boxen und Beisiele von hier](https://github.com/Wandmalfarbe/pandoc-latex-template)

Die Vorlage muss sich im Pandoc-Benutzerverzeichnis befinden.

Wenn es nicht existiert, muss es erstellt und die Datei dort abgelegt werden:

```
# cd ~
# mkdir .pandoc && cd .pandoc
# mkdir templates && cd templates
# cp <Open-University-Pandoc>eisvogel.latex eisvogel.latex
```

# Der Befehl zum Erstellen einer PDF-Datei aus der Quelle lautet:

Generisch:
```
pandoc <inputdatei.md> -o <outputdatei.pdf> --from markdown --template eisvogel --filter pandoc-latex-environment --filter pandoc-include --listing --citeproc
```
Für die GesamtDoku:
```
pandoc --from markdown --template eisvogel --filter pandoc-latex-environment --filter pandoc-include --listing --citeproc -o GesamtDoku.pdf GesamtDoku.md
```

# Eine PDF-Datei aus einem anderen Programm kann ohne Rahmen geplottet und dann zugeschnitten werden.

Geht mit pdfCropMargins.

```
pdf-crop-margins -v -s -u your-file.pdf
```

Zur Hilfe:

```
pdf-crop-margins -h | more
```

# Installieren der Programme und Abhängigkeiten
// sudo apt install pandoc !!! Das ist eigentlich eine schlechte Idee, es installiert die Pandoc-Version vor 5 Jahren.
Die Befehlszeile, wenn ich „pandoc“ auf dem UBUNTU-Terminal eingebe, lautet
"use apt install pandoc"... Es ist eine schlechte Idee!
pandoc muss wieder deinstalliert werden.

```
apt purge pandoc
```

Sich das frisches .deb bei Pandocs Git/Releases holen:

```
wget https://github.com/jgm/pandoc/releases/download/3.2.1/pandoc-3.2.1-1-amd64.deb
```

Installiere das:

```
sudo dpkg -i pandoc-3.2.1-1-amd64.deb
```


Die anderen Pakete sind ganz ok:

```
sudo apt-get install texlive-latex-base // for pdflatex
sudo apt-get install texlive-fonts-recommended texlive-fonts-extra
sudo apt-get install texlive-fonts-extra
sudo apt-get install texlive-latex-extra
sudo apt install texlive-full // pretty big for awesomebox

one line:
sudo apt-get install texlive-latex-base texlive-fonts-recommended texlive-fonts-extra texlive-latex-extra texlive-full

pip install pandoc-latex-environment
pip install pandoc-include

```

Das crop tool installieren:

```
pip install pdfCropMargins
```

# weiterführende Links
https://blog.cubieserver.de/2021/document-writing-with-markdown-and-latex/
https://danielwiese.com/posts/documentation/
https://www.zettlr.com/
