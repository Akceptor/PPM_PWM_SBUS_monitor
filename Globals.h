#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>

#define SIGNAL_PIN 19
#define BUTTON_PIN 0
#define LONG_PRESS_MS 3000

enum SignalType {
  NONE,
  PWM,
  PPM,
  SBUS,
  CRSF
};

extern SignalType active_type;
extern SignalType selected_type;

extern bool sbus_inverted;
extern bool crsf_inverted;
extern uint32_t crsf_baud;
extern uint16_t ppm_frame_ms;
extern uint8_t ppm_channels;
extern uint16_t ppm_stop_us;
extern bool ppm_is_positive;

extern volatile uint16_t channels[16];
extern volatile bool signal_detected;
extern volatile uint32_t last_edge_time;
extern volatile uint32_t last_high_duration;
extern volatile uint32_t last_cycle_time;
extern volatile uint32_t sbus_valid_frame_count;
extern volatile uint32_t crsf_valid_frame_count;
extern bool serial_active;

extern HardwareSerial SerialRX;
#endif