#include "Globals.h"

SignalType active_type = NONE;
SignalType selected_type = NONE;

bool sbus_inverted = true;
bool crsf_inverted = false;
uint32_t crsf_baud = 420000;
uint16_t ppm_frame_ms = 225;
uint8_t ppm_channels = 8;
uint16_t ppm_stop_us = 300;
bool ppm_is_positive = true;

volatile uint16_t channels[16] = {0};
volatile bool signal_detected = false;
volatile uint32_t last_edge_time = 0;
volatile uint32_t last_high_duration = 0;
volatile uint32_t last_cycle_time = 0;
volatile uint32_t sbus_valid_frame_count = 0;
volatile uint32_t crsf_valid_frame_count = 0;
bool serial_active = false;

HardwareSerial SerialRX(2);