#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Globals.h"
#include "PPM_PWM.h"
#include "SBUS.h"
#include "CRSF.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C
#define OLED_SDA 21
#define OLED_SCL 22

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

SignalType active_type = NONE;
SignalType selected_type = PWM;

void stopAllProtocols() {
  SerialRX.end();
  detachInterrupt(digitalPinToInterrupt(SIGNAL_PIN));
  serial_active = false;
  active_type = NONE;
  sbus_valid_frame_count = 0;
  crsf_valid_frame_count = 0;
  signal_detected = false;
  for(int i=0; i<8; i++) channels[i] = 0;
}

void startSelectedProtocol() {
  stopAllProtocols();
  active_type = selected_type;
  if (active_type == PWM || active_type == PPM) {
    attachInterrupt(digitalPinToInterrupt(SIGNAL_PIN), handleInterrupt, CHANGE);
  } else if (active_type == SBUS_NON_INV) {
    startSBUS(false);
  } else if (active_type == SBUS_INV) {
    startSBUS(true);
  } else if (active_type == CRSF) {
    startCRSF(false);
  } else if (active_type == CRSF_INV) {
    startCRSF(true);
  }
}

void cycleSelection() {
  int next = (int)selected_type + 1;
  if (next > (int)CRSF_INV) next = 1; 
  selected_type = (SignalType)next;
}

const char* getTypeName(SignalType type) {
  switch (type) {
    case PWM: return "PWM";
    case PPM: return "PPM";
    case SBUS_NON_INV: return "SBUS";
    case SBUS_INV: return "SBUS INV";
    case CRSF: return "CRSF";
    case CRSF_INV: return "CRSF INV";
    default: return "NONE";
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  pinMode(SIGNAL_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  static uint32_t pressStart = 0;
  static bool lastBtn = HIGH;
  bool currentBtn = digitalRead(BUTTON_PIN);

  if (lastBtn == HIGH && currentBtn == LOW) {
    pressStart = millis();
  } else if (lastBtn == LOW && currentBtn == HIGH) {
    uint32_t duration = millis() - pressStart;
    if (duration > 2000) {
      startSelectedProtocol();
    } else if (duration > 50) {
      if (active_type == NONE) cycleSelection();
      else stopAllProtocols();
    }
    pressStart = 0;
  }
  lastBtn = currentBtn;

  if (serial_active) {
    if (active_type == SBUS_NON_INV || active_type == SBUS_INV) {
      decodeSBUS();
    } else if (active_type == CRSF || active_type == CRSF_INV) {
      decodeCRSF();
    }
  }

  static uint32_t lastDisplayUpdate = 0;
  if (millis() - lastDisplayUpdate > 100) {
    lastDisplayUpdate = millis();
    display.clearDisplay();
    display.setCursor(0, 0);

    if (active_type == NONE) {
      display.println("MODE SELECTION");
      display.println("----------------");
      display.print(" > ");
      display.println(getTypeName(selected_type));
      display.println("\nShort: Cycle");
      display.println("Long: Enable");
    } else {
      display.print("Active: ");
      display.println(getTypeName(active_type));

      bool validSignal = signal_detected && (micros() - last_edge_time < 500000);
      if (!validSignal) {
        display.println("\nNO SIGNAL");
      } else {
        switch (active_type) {
        case PWM:
          display.print("Freq: ");
          display.print(last_cycle_time > 0 ? 1000000.0 / last_cycle_time : 0, 1);
          display.println("Hz");
          display.print("Width: ");
          display.println(last_high_duration);
          break;
        case PPM:
        case SBUS_NON_INV:
        case SBUS_INV:
        case CRSF:
        case CRSF_INV:
          for (int i = 0; i < 8; i++) {
            int col = (i % 2) * 64;
            int row = 16 + (i / 2) * 10;
            display.setCursor(col, row);
            display.printf("C%d:%d", i + 1, channels[i]);
          }
          if (active_type >= SBUS_NON_INV) {
            display.setCursor(0, 56);
            display.printf("PKT: %d", (active_type >= CRSF) ? crsf_valid_frame_count : sbus_valid_frame_count);
          }
          break;
        }
      }
    }
    display.display();
  }
}