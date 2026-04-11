#pragma once

#include <Arduino.h>

#define FIRMWARE_VERSION "1.0.0"

namespace cfg {

constexpr int kScreenWidth = 320;
constexpr int kScreenHeight = 820;

constexpr int kPinBacklight = 6;

constexpr int kPinLcdCs = 0;
constexpr int kPinLcdSck = 2;
constexpr int kPinLcdMosi = 1;
constexpr int kPinLcdReset = 16;

constexpr int kPinHsync = 38;
constexpr int kPinVsync = 39;
constexpr int kPinDe = 40;
constexpr int kPinPclk = 41;

constexpr int kPinR0 = 17;
constexpr int kPinR1 = 46;
constexpr int kPinR2 = 3;
constexpr int kPinR3 = 8;
constexpr int kPinR4 = 18;

constexpr int kPinG0 = 14;
constexpr int kPinG1 = 13;
constexpr int kPinG2 = 12;
constexpr int kPinG3 = 11;
constexpr int kPinG4 = 10;
constexpr int kPinG5 = 9;

constexpr int kPinB0 = 21;
constexpr int kPinB1 = 5;
constexpr int kPinB2 = 45;
constexpr int kPinB3 = 48;
constexpr int kPinB4 = 47;

constexpr uint32_t kRgbClockHz = 18000000;

constexpr uint8_t kBacklightDefaultBrightness = 255;
constexpr uint8_t kBacklightMinBrightness = 0;
constexpr uint8_t kBacklightMaxBrightness = 255;
constexpr uint32_t kBacklightPwmHz = 50000;
constexpr uint8_t kBacklightPwmResolution = 8;
constexpr uint8_t kBacklightPwmChannel = 1;

inline uint32_t backlightDutyFromBrightness(uint8_t brightness) {
  return 0xFFu - brightness;
}

}  // namespace cfg
