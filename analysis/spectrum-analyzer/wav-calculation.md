# Dauer der ADC-Daten

Die Dauer der ADC-Daten hängt direkt von der **Abtastrate** und der **Anzahl der gesammelten Samples** ab.

## Gegeben:
- **Abtastrate (`SAMPLE_RATE`)**: 44.1 kHz = 44,100 Samples/Sekunde
- **Anzahl der Samples (`FFT_SIZE`)**: 1024

## Berechnung:
Die Dauer der ADC-Daten in Millisekunden ergibt sich aus der Anzahl der Samples dividiert durch die Abtastrate:

\[
\text{Dauer in Sekunden} = \frac{\text{FFT\_SIZE}}{\text{SAMPLE\_RATE}}
\]

\[
\text{Dauer in Millisekunden} = \frac{\text{FFT\_SIZE}}{\text{SAMPLE\_RATE}} \times 1000
\]

### Einsetzen der Werte:

\[
\text{Dauer in Millisekunden} = \frac{1024}{44100} \times 1000 \approx 23.22 \, \text{ms}
\]

## Ergebnis:
- **Eine Sammlung von 1024 Samples entspricht etwa 23.22 ms an Daten.**

### Für den Ringpuffer:
Wenn beispielsweise 10 dieser Buffers im Ringpuffer gespeichert werden, entspricht das:

\[
10 \times 23.22 \, \text{ms} \approx 232.2 \, \text{ms.}
\]

Das wären knapp **1/4 Sekunde an Audiodaten**.
