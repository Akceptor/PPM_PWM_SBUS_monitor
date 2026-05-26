# PPM / PWM / SBUS / CRSF Monitor

ESP32-based signal monitor for reading and inspecting RC receiver outputs, including **PPM/CPPM**, **PWM**, **CRSF**, and **SBUS** signals.

## Overview

This project appears to provide a small embedded utility for monitoring common radio-control receiver output formats. It is intended to help inspect or debug signals coming from:

- **PWM** channel outputs - single channel
- **PPM / CPPM** trainer-port style summed signals
- **SBUS** serial receiver output
- **CRSF** receiver output / S.Port output from EdgeTX Handset (configurable baud rate)

## Features

- Monitor common RC signal protocols in one project
- Inspect **CPPM/PPM**, **PWM**, **CRSF**, and **SBUS** signals
- Useful for testing receiver wiring and output behavior
- Helpful as a debugging tool when working with RC transmitters and receivers

## Hardware notes

The original repository note included the following trainer-port reference:

### CPPM
- **Trainer port pinout:** https://i.imgur.com/k5L9x3El.jpg
- Use **Slave** mode if you want to output
- EdgeTX JR bay doesn't work...
  
### SBUS
- Use ELRS receiver output or CPPM pin from JR-Bay

### CRSF
- Handset output is inversed

## Wiring
### OLED:
 - SDA -> pin 21
 - SLC -> pin 22
### DATA
- Signal -> pin 19
- Button -> pin 0
