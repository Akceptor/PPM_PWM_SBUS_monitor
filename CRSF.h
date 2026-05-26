#ifndef CRSF_H
#define CRSF_H
#include <Arduino.h>
void startCRSF(uint32_t baud, bool inverted);
void decodeCRSF();
#endif