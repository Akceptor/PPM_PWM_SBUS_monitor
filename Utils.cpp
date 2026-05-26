#include "Utils.h"
#include "Globals.h"

bool unpackChannels(uint8_t *buffer) {
  channels[0]  = ((buffer[0]       | buffer[1] << 8) & 0x07FF);
  channels[1]  = ((buffer[1] >> 3  | buffer[2] << 5) & 0x07FF);
  channels[2]  = ((buffer[2] >> 6  | buffer[3] << 2 | buffer[4] << 10) & 0x07FF);
  channels[3]  = ((buffer[4] >> 1  | buffer[5] << 7) & 0x07FF);
  channels[4]  = ((buffer[5] >> 4  | buffer[6] << 4) & 0x07FF);
  channels[5]  = ((buffer[6] >> 7  | buffer[7] << 1 | buffer[8] << 9) & 0x07FF);
  channels[6]  = ((buffer[8] >> 2  | buffer[9] << 6) & 0x07FF);
  channels[7]  = ((buffer[9] >> 5  | buffer[10] << 3) & 0x07FF);
  channels[8]  = ((buffer[11]      | buffer[12] << 8) & 0x07FF);
  channels[9]  = ((buffer[12] >> 3 | buffer[13] << 5) & 0x07FF);
  channels[10] = ((buffer[13] >> 6 | buffer[14] << 2 | buffer[15] << 10) & 0x07FF);
  channels[11] = ((buffer[15] >> 1 | buffer[16] << 7) & 0x07FF);
  channels[12] = ((buffer[16] >> 4 | buffer[17] << 4) & 0x07FF);
  channels[13] = ((buffer[17] >> 7 | buffer[18] << 1 | buffer[19] << 9) & 0x07FF);
  channels[14] = ((buffer[19] >> 2 | buffer[20] << 6) & 0x07FF);
  channels[15] = ((buffer[20] >> 5 | buffer[21] << 3) & 0x07FF);
  return true;
}

uint8_t crsf_crc8(const uint8_t *ptr, uint8_t len) {
  uint8_t crc = 0;
  while (len--) {
    crc ^= *ptr++;
    for (uint8_t i = 0; i < 8; i++) {
      if (crc & 0x80) {
        crc = (crc << 1) ^ 0xD5;
      }
      else {
        crc <<= 1;
      }
    }
  }
  return crc;
}