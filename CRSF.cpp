#include "CRSF.h"
#include "Globals.h"
#include "Utils.h"

#define CRSF_ADDR_FLIGHT_CONTROLLER 0xC8
#define CRSF_ADDR_RECEIVER          0xEC
#define CRSF_ADDR_TRANSMITTER       0xEA
#define CRSF_FRAME_RC_CHANNELS_PACKED 0x16
#define CRSF_RC_FRAME_LENGTH 24

void startCRSF(uint32_t baud, bool inverted) {
  delay(20);
  SerialRX.begin(baud, SERIAL_8N1, SIGNAL_PIN, -1, inverted);
  serial_active = true;
}

void decodeCRSF() {
  while (SerialRX.available() >= 4) {
    uint8_t addr = SerialRX.peek();
    if (addr != CRSF_ADDR_FLIGHT_CONTROLLER &&
        addr != CRSF_ADDR_RECEIVER &&
        addr != CRSF_ADDR_TRANSMITTER) {
      SerialRX.read();
      continue;
    }

    uint8_t device_addr = SerialRX.read();
    if (SerialRX.available() < 1) return;
    
    uint8_t frame_length = SerialRX.read();
    if (frame_length < 2 || frame_length > 62) continue;
    if (SerialRX.available() < frame_length) return;

    uint8_t frame_buffer[64];
    if (SerialRX.readBytes(frame_buffer, frame_length) != frame_length) return;

    uint8_t frame_type = frame_buffer[0];
    uint8_t received_crc = frame_buffer[frame_length - 1];
    uint8_t calculated_crc = crsf_crc8(frame_buffer, frame_length - 1);

    if (calculated_crc != received_crc) continue;

    if (frame_type == CRSF_FRAME_RC_CHANNELS_PACKED &&
        frame_length == CRSF_RC_FRAME_LENGTH) {
      unpackChannels(&frame_buffer[1]);
      crsf_valid_frame_count++;
      signal_detected = true;
      last_edge_time = micros();
    }
  }
}