#ifndef UTILS_H
#define UTILS_H
#include <Arduino.h>
bool unpackChannels(uint8_t *buffer);
uint8_t crsf_crc8(const uint8_t *ptr, uint8_t len);
#endif