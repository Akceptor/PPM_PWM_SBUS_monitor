# PPM / PWM / SBUS Monitor

Arduino-based signal monitor for reading and inspecting RC receiver outputs, including **PPM/CPPM**, **PWM**, and **SBUS** signals.

## Overview

This project appears to provide a small embedded utility for monitoring common radio-control receiver output formats. It is intended to help inspect or debug signals coming from:

- **PWM** channel outputs
- **PPM / CPPM** trainer-port style summed signals
- **SBUS** serial receiver output

The repository is primarily written in **C++** with a small amount of **C**, which is typical for Arduino or microcontroller-based projects.

## Features

- Monitor common RC signal protocols in one project
- Inspect **CPPM/PPM**, **PWM**, and **SBUS** signals
- Useful for testing receiver wiring and output behavior
- Helpful as a debugging tool when working with RC transmitters and receivers

## Hardware notes

The original repository note included the following trainer-port reference:

- **CPPM Trainer port pinout:** https://i.imgur.com/k5L9x3El.jpg
- Use **Slave** mode if you want to output

## Repository information

- **Repository:** `Akceptor/PPM_PWM_SBUS_monitor`
- **Languages:** C++ and C

## Getting started

1. Clone the repository.
2. Open the project in the Arduino IDE or another compatible embedded toolchain.
3. Build and upload the firmware to your target board.
4. Connect a compatible RC signal source:
   - PWM
   - PPM / CPPM
   - SBUS
5. Open the serial monitor or other configured output to observe decoded values.

## Typical use cases

- Debugging RC receiver wiring
- Verifying trainer-port / CPPM signals
- Inspecting SBUS receiver output
- Comparing different receiver output formats

## Notes

This README was generated from the repository context and existing content. It can be improved further by extracting exact technical details from the source code, such as:

- supported microcontroller boards
- exact pin assignments
- serial baud rate
- sample serial output
- protocol decoding behavior
- upload and usage instructions
