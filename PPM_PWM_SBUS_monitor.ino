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

enum MenuState { SELECT_TYPE, CONFIG_SUB1, CONFIG_SUB2, CONFIG_SUB3, CONFIG_SUB4 };
MenuState menuState = SELECT_TYPE;

uint32_t bauds[] = {420000, 400000, 115200};
uint8_t baud_idx = 0;
uint8_t ppm_ch_opts[] = {8, 12, 16};
uint8_t ppm_ch_idx = 0;

void stopAllProtocols() {
  SerialRX.end();
  detachInterrupt(digitalPinToInterrupt(SIGNAL_PIN));
  serial_active = false;
  active_type = NONE;
  menuState = SELECT_TYPE;
  sbus_valid_frame_count = 0;
  crsf_valid_frame_count = 0;
  signal_detected = false;
  for(int i=0; i<16; i++) channels[i] = 0;
}

void startSelectedProtocol() {
  stopAllProtocols();
  active_type = selected_type;
  if (active_type == PWM || active_type == PPM) {
    attachInterrupt(digitalPinToInterrupt(SIGNAL_PIN), handleInterrupt, CHANGE);
  } else if (active_type == SBUS) {
    startSBUS(sbus_inverted);
  } else if (active_type == CRSF) {
    startCRSF(crsf_baud, crsf_inverted);
  }
}

void cycleSelection() {
  int next = (int)selected_type + 1;
  if (next > (int)CRSF) next = 1; 
  selected_type = (SignalType)next;
}

const char* getTypeName(SignalType type) {
  switch (type) {
    case PWM: return "PWM";
    case PPM: return "PPM";
    case SBUS: return "SBUS";
    case CRSF: return "CRSF";
    default: return "NONE";
  }
}

void handleShortPress() {
  if (menuState == SELECT_TYPE) {
    cycleSelection();
  } else if (menuState == CONFIG_SUB1) {
    if (selected_type == SBUS) sbus_inverted = !sbus_inverted;
    else if (selected_type == CRSF) crsf_inverted = !crsf_inverted;
    else if (selected_type == PPM) {
      ppm_ch_idx = (ppm_ch_idx + 1) % 3;
      ppm_channels = ppm_ch_opts[ppm_ch_idx];
    }
  } else if (menuState == CONFIG_SUB2) {
    if (selected_type == CRSF) {
      baud_idx = (baud_idx + 1) % 3;
      crsf_baud = bauds[baud_idx];
    } else if (selected_type == PPM) {
      ppm_frame_ms += 5; // Step 0.5ms (stored as 5)
      if (ppm_frame_ms > 300) ppm_frame_ms = 200; // Range 20.0 - 30.0ms
    }
  } else if (menuState == CONFIG_SUB3) {
    if (selected_type == PPM) {
      ppm_stop_us += 50; // Step 50us
      if (ppm_stop_us > 550) ppm_stop_us = 250; // Range 250 - 550us
    }
  } else if (menuState == CONFIG_SUB4) {
    if (selected_type == PPM) {
      ppm_is_positive = !ppm_is_positive;
    }
  }
}

void handleLongPress() {
  if (menuState == SELECT_TYPE) {
    if (selected_type == PWM) startSelectedProtocol();
    else menuState = CONFIG_SUB1;
  } else if (menuState == CONFIG_SUB1) {
    if (selected_type == SBUS) startSelectedProtocol();
    else menuState = CONFIG_SUB2;
  } else if (menuState == CONFIG_SUB2) {
    if (selected_type == CRSF) startSelectedProtocol();
    else menuState = CONFIG_SUB3;
  } else if (menuState == CONFIG_SUB3) {
    menuState = CONFIG_SUB4;
  } else if (menuState == CONFIG_SUB4) {
    startSelectedProtocol();
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
    if (duration > LONG_PRESS_MS) {
      if (active_type == NONE) handleLongPress();
      else stopAllProtocols();
    } else if (duration > 50) {
      if (active_type == NONE) handleShortPress();
      else stopAllProtocols();
    }
    pressStart = 0;
  }
  lastBtn = currentBtn;

  if (serial_active) {
    if (active_type == SBUS) {
      decodeSBUS();
    } else if (active_type == CRSF) {
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
      if (menuState == SELECT_TYPE) {
        display.println(getTypeName(selected_type));
      } else if (menuState == CONFIG_SUB1) {
        display.print(getTypeName(selected_type));
        display.println(" CFG (1/2)");
        if (selected_type == SBUS || selected_type == CRSF) {
          display.print(" Invert: ");
          display.println((selected_type == SBUS ? sbus_inverted : crsf_inverted) ? "YES" : "NO");
        } else if (selected_type == PPM) {
          display.print(" Channels: ");
          display.println(ppm_channels);
        }
      } else if (menuState == CONFIG_SUB2) {
        display.print(getTypeName(selected_type));
        display.printf(" CFG (%d/%d)\n", 2, (selected_type == PPM ? 4 : 2));
        if (selected_type == CRSF) {
          display.print(" Baud: ");
          display.println(crsf_baud);
        } else if (selected_type == PPM) {
          display.print(" Frame: ");
          display.print(ppm_frame_ms / 10.0, 1);
          display.println("ms");
        }
      } else if (menuState == CONFIG_SUB3) {
        display.print(getTypeName(selected_type));
        display.println(" CFG (3/4)");
        if (selected_type == PPM) {
          display.print(" Stop: ");
          display.print(ppm_stop_us);
          display.println("us");
        }
      } else if (menuState == CONFIG_SUB4) {
        display.print(getTypeName(selected_type));
        display.println(" CFG (4/4)");
        if (selected_type == PPM) {
          display.print(" Pol: ");
          display.println(ppm_is_positive ? "POS (+)" : "NEG (-)");
        }
      }
      display.setCursor(0, 48);
      display.println("Short: Next/Cycle");
      display.println("Long: OK/Start");
    } else {
      display.print("Active: ");
      display.print(getTypeName(active_type));
      if (active_type == SBUS) display.print(sbus_inverted ? " (INV)" : " (NON)");
      if (active_type == CRSF) {
        display.print(crsf_inverted ? " (INV) " : " (NON) ");
        display.printf("%dk", crsf_baud / 1000);
      }
      if (active_type == PPM) {
        display.print(ppm_is_positive ? " (+)" : " (-)");
        display.printf(" %dus", ppm_stop_us);
      }
      display.println();

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
        case SBUS:
        case CRSF:
          for (int i = 0; i < (active_type == PPM ? ppm_channels : 12); i++) {
            if (i >= 12) break; // Display limit
            int col = (i % 2) * 64;
            int row = 16 + (i / 2) * 10;
            display.setCursor(col, row);
            display.printf("C%d:%d", i + 1, channels[i]);
          }
          if (active_type >= SBUS) {
            display.setCursor(0, 56);
            display.printf("PKT: %d", (active_type == CRSF) ? crsf_valid_frame_count : sbus_valid_frame_count);
          }
          break;
        }
      }
    }

    // Draw progress bar for long press duration
    if (pressStart != 0 && currentBtn == LOW) {
      uint32_t elapsed = millis() - pressStart;
      if (elapsed > 100) { // Only show after initial debounce
        int barWidth = (elapsed * SCREEN_WIDTH) / LONG_PRESS_MS;
        if (barWidth > SCREEN_WIDTH) barWidth = SCREEN_WIDTH;
        display.fillRect(0, SCREEN_HEIGHT - 2, barWidth, 2, SSD1306_WHITE);
      }
    }

    display.display();
  }
}