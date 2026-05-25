#include "PPM_PWM.h"
#include "Globals.h"
#include "soc/gpio_struct.h"

volatile uint8_t current_channel = 0;
volatile unsigned long last_rising_time = 0;

void IRAM_ATTR handleInterrupt() {
  uint32_t now = micros();
  uint32_t duration = now - last_edge_time;
  last_edge_time = now;

  bool pinState = GPIO.in & (1 << SIGNAL_PIN);
  signal_detected = true;

  if (pinState) { // Rising edge
    if (duration > 2500) {
      current_channel = 0;
    } else {
      if (active_type == PPM && current_channel < 8 && last_rising_time > 0) {
        channels[current_channel] = now - last_rising_time;
        current_channel++;
      }
    }

    if (last_rising_time > 0) {
      last_cycle_time = now - last_rising_time;
    }
    last_rising_time = now;
  } else { // Falling edge
    last_high_duration = duration;
    if (active_type == PWM) {
      channels[0] = duration;
    }
  }
}