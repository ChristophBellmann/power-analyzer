# 2. Team

Stellt sich jeder mal vor. 

Wenn wir eine Projektmanagement-Software nutzen (OpenProject?) dann hier das Gantt-Diagramm hin.  

\break

# 4. Anforderungen

Allgemein Anforderungen: Leistungsaufnahme einer Kaffeemaschine messen.

: Was soll gemessen werden:

 | Größe | Wert [min-max] | Einheit |
 |----------------|-------------------|----------------|
 |Spannung: | 0 - 300 | V|
 |Strom: | 0 - 5 | A|
 |Frequenz: | 0 - 200 | kHz|
 Details in der Spezifikation.

\break

# 5. Spezifikation

Die Kaffeemaschine verursacht Netzrückwirkungen. Wir stellen fest, ob die Höhe der Oberwellen noch innerhalb der Norm liegt. 

## 5.1 Idee und Lösung

Analoges Frontend für AC Spannugn + Strom, digital erfassen mit dem eingebauten ADC von einem ESP32 Mikrocontroller..

### 5.1.1 Elektronik

  1) Schaltung Messung AC Spannung:

  - Trenntrafo
  - Operationsverstärker

  2)  Schaltung Messung AC Strom:

  - Hall-Effekt Spule
  - Operationsverstärker

### 5.1.2 Software

Basierend auf ESP32.

Hardware-Limiterungen:

- Prozessor:240 Mhz Dualcore
- Speicher
    - Flash: 4 MB oder mehr/weniger  
    - RAM: 4 MB oder mehr/weniger  


\break