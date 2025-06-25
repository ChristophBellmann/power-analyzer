# 2. Team

- Softwareentwickler  
Programmierung in c, web

- Projektmanager  
OpenProject, Gantt-Diagramm

- Qualitätsmanagement  

- Marketing  

\break

# 4. Anforderungen

Allgemein Anforderungen: Internetverbindung

: Was soll gemessen werden:

 | Größe | Wert [min-max] | Einheit |
 |----------------|-------------------|----------------|
 |Spannung: | 0 - 300 | V|
 |Strom: | 0 - 5 | A|
 |Frequenz: | 0 - 200 | kHz|
 Details in der Spezifikation.

\break

# 5. Spezifikation

Ein Gerät verursacht Netzrückwirkungen. Es ist festzustellen, ob die Höhe der Oberwellen noch innerhalb der Norm liegt. 

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
