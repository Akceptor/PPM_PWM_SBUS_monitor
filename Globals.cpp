#include "Globals.h"

volatile uint16_t channels[8] = {0};
volatile bool signal_detected = false;
volatile uint32_t last_edge_time = 0;
volatile uint32_t last_high_duration = 0;
volatile uint32_t last_cycle_time = 0;
volatile uint32_t sbus_valid_frame_count = 0;
volatile uint32_t crsf_valid_frame_count = 0;
bool serial_active = false;

HardwareSerial SerialRX(2);