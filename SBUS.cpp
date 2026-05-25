#include "SBUS.h"
#include "Globals.h"
#include "Utils.h"

void startSBUS(bool inverted) {
  delay(20);
  SerialRX.begin(100000, SERIAL_8E2, SIGNAL_PIN, -1, inverted);
  serial_active = true;
}

bool decodeSBUSFrame(uint8_t *buffer) {
  if (buffer[0] != 0x0F) return false;
  if ((buffer[24] & 0x0F) != 0x00) return false;
  return unpackChannels(&buffer[1]);
}

void decodeSBUS() {
  while (SerialRX.available() >= 25) {
    if (SerialRX.peek() != 0x0F) {
      SerialRX.read();
      continue;
    }

    uint8_t buffer[25];
    if (SerialRX.readBytes(buffer, 25) != 25) return;

    if (decodeSBUSFrame(buffer)) {
      sbus_valid_frame_count++;
      signal_detected = true;
      last_edge_time = micros();
    }
  }
}