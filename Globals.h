#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>

#define SIGNAL_PIN 19
#define BUTTON_PIN 0

enum SignalType {
  NONE,
  PWM,
  PPM,
  SBUS_NON_INV,
  SBUS_INV,
  CRSF,
  CRSF_INV
};

extern SignalType active_type;
extern SignalType selected_type;
extern volatile uint16_t channels[8];
extern volatile bool signal_detected;
extern volatile uint32_t last_edge_time;
extern volatile uint32_t last_high_duration;
extern volatile uint32_t last_cycle_time;
extern volatile uint32_t sbus_valid_frame_count;
extern volatile uint32_t crsf_valid_frame_count;
extern bool serial_active;

extern HardwareSerial SerialRX;
#endif