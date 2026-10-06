# Hardware

## ESP32 platform

The firmware is built around the ESP32 and uses continuous ADC acquisition together with DMA-backed buffering. Signal-processing and web-server tasks can therefore operate on blocks of samples instead of servicing every ADC sample individually.

## Voltage measurement

The original project concept uses an isolated voltage measurement front end based on a measurement transformer and analogue conditioning.

![ZMPT101B voltage transformer](../doc/02-pictures/ZMPT101B.png)

## Current measurement

Current is measured without a direct conductive connection using a current transformer and analogue conditioning.

![ZMCT103C current transformer](../doc/02-pictures/ZMCT103C.png)

## Analogue front end

Both measurement paths require conditioning appropriate to the ESP32 ADC input range. The exact component values, calibration and protection should be verified against the hardware revision being used.

> **Mains safety:** Galvanic isolation, creepage/clearance, fusing, enclosure design and component ratings must be appropriate for the installation. Do not connect an ESP32 ADC directly to mains voltage.
