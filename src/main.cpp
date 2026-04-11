#define LGFX_USE_V1

#include <Arduino.h>
#include <driver/ledc.h>
#include <LovyanGFX.hpp>
#include "config.h"
#include "debug.h"
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>

namespace {

void initBacklight(uint8_t brightness) {
  ledc_timer_config_t timer_conf = {
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .duty_resolution = static_cast<ledc_timer_bit_t>(cfg::kBacklightPwmResolution),
      .timer_num = LEDC_TIMER_3,
      .freq_hz = cfg::kBacklightPwmHz,
      .clk_cfg = LEDC_AUTO_CLK,
  };

  ledc_channel_config_t channel_conf = {
      .gpio_num = cfg::kPinBacklight,
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .channel = static_cast<ledc_channel_t>(cfg::kBacklightPwmChannel),
      .intr_type = LEDC_INTR_DISABLE,
      .timer_sel = LEDC_TIMER_3,
      .duty = cfg::backlightDutyFromBrightness(brightness),
      .hpoint = 0,
  };

  ledc_timer_config(&timer_conf);
  ledc_channel_config(&channel_conf);
}

void setBacklightBrightness(uint8_t brightness) {
  const uint32_t duty = cfg::backlightDutyFromBrightness(brightness);
  ledc_set_duty(LEDC_LOW_SPEED_MODE,
                static_cast<ledc_channel_t>(cfg::kBacklightPwmChannel),
                duty);
  ledc_update_duty(LEDC_LOW_SPEED_MODE,
                   static_cast<ledc_channel_t>(cfg::kBacklightPwmChannel));
}

class WaveshareDisplay : public lgfx::LGFX_Device {
 public:
  class WavesharePanel : public lgfx::Panel_ST7701_Base {
   protected:
    const uint8_t *getInitCommands(uint8_t listno) const override {
      static constexpr const uint8_t list0[] = {
          0xFF, 5, 0x77, 0x01, 0x00, 0x00, 0x13,
          0xEF, 1, 0x08,
          0xFF, 5, 0x77, 0x01, 0x00, 0x00, 0x10,
          0xC0, 2, 0xE5, 0x02,
          0xC1, 2, 0x15, 0x0A,
          0xC2, 2, 0x07, 0x02,
          0xCC, 1, 0x10,
          0xB0, 16, 0x00, 0x08, 0x51, 0x0D, 0xCE, 0x06, 0x00, 0x08,
                    0x08, 0x24, 0x05, 0xD0, 0x0F, 0x6F, 0x36, 0x1F,
          0xB1, 16, 0x00, 0x10, 0x4F, 0x0C, 0x11, 0x05, 0x00, 0x07,
                    0x07, 0x18, 0x02, 0xD3, 0x11, 0x6E, 0x34, 0x1F,
          0xFF, 5, 0x77, 0x01, 0x00, 0x00, 0x11,
          0xB0, 1, 0x4D,
          0xB1, 1, 0x37,
          0xB2, 1, 0x87,
          0xB3, 1, 0x80,
          0xB5, 1, 0x4A,
          0xB7, 1, 0x85,
          0xB8, 1, 0x21,
          0xB9, 2, 0x00, 0x13,
          0xC0, 1, 0x09,
          0xC1, 1, 0x78,
          0xC2, 1, 0x78,
          0xD0, 1, 0x88,
          0xE0, 3 + CMD_INIT_DELAY, 0x80, 0x00, 0x02, 100,
          0xE1, 11, 0x0F, 0xA0, 0x00, 0x00, 0x10, 0xA0, 0x00, 0x00, 0x00, 0x60, 0x60,
          0xE2, 13, 0x30, 0x30, 0x60, 0x60, 0x45, 0xA0, 0x00, 0x00, 0x46, 0xA0, 0x00, 0x00, 0x00,
          0xE3, 4, 0x00, 0x00, 0x33, 0x33,
          0xE4, 2, 0x44, 0x44,
          0xE5, 16, 0x0F, 0x4A, 0xA0, 0xA0, 0x11, 0x4A, 0xA0, 0xA0,
                    0x13, 0x4A, 0xA0, 0xA0, 0x15, 0x4A, 0xA0, 0xA0,
          0xE6, 4, 0x00, 0x00, 0x33, 0x33,
          0xE7, 2, 0x44, 0x44,
          0xE8, 16, 0x10, 0x4A, 0xA0, 0xA0, 0x12, 0x4A, 0xA0, 0xA0,
                    0x14, 0x4A, 0xA0, 0xA0, 0x16, 0x4A, 0xA0, 0xA0,
          0xEB, 7, 0x02, 0x00, 0x4E, 0x4E, 0xEE, 0x44, 0x00,
          0xED, 16, 0xFF, 0xFF, 0x04, 0x56, 0x72, 0xFF, 0xFF, 0xFF,
                    0xFF, 0xFF, 0xFF, 0x27, 0x65, 0x40, 0xFF, 0xFF,
          0xEF, 6, 0x08, 0x08, 0x08, 0x40, 0x3F, 0x64,
          0xFF, 5, 0x77, 0x01, 0x00, 0x00, 0x13,
          0xE8, 2, 0x00, 0x0E,
          0xFF, 5, 0x77, 0x01, 0x00, 0x00, 0x00,
          0x11, 0 + CMD_INIT_DELAY, 120,
          0xFF, 5, 0x77, 0x01, 0x00, 0x00, 0x13,
          0xE8, 2 + CMD_INIT_DELAY, 0x00, 0x0C, 10,
          0xE8, 2, 0x00, 0x00,
          0xFF, 5, 0x77, 0x01, 0x00, 0x00, 0x00,
          0x3A, 1, 0x55,
          0x36, 1, 0x00,
          0x35, 1, 0x00,
          0x29, 0 + CMD_INIT_DELAY, 20,
          0xFF, 0xFF,
      };

      switch (listno) {
        case 0:
          return list0;
        default:
          return nullptr;
      }
    }
  };

  WaveshareDisplay() {
    {
      auto cfg = panel_.config();
      cfg.pin_cs = cfg::kPinLcdCs;
      cfg.pin_rst = cfg::kPinLcdReset;
      cfg.memory_width = cfg::kScreenWidth;
      cfg.memory_height = cfg::kScreenHeight;
      cfg.panel_width = cfg::kScreenWidth;
      cfg.panel_height = cfg::kScreenHeight;
      cfg.offset_x = 0;
      cfg.offset_y = 0;
      cfg.rgb_order = true;
      cfg.invert = false;
      panel_.config(cfg);
    }

    {
      auto cfg = panel_.config_detail();
      cfg.pin_cs = cfg::kPinLcdCs;
      cfg.pin_sclk = cfg::kPinLcdSck;
      cfg.pin_mosi = cfg::kPinLcdMosi;
      cfg.use_psram = 1;
      panel_.config_detail(cfg);
    }

    {
      auto cfg = bus_.config();
      cfg.panel = &panel_;

      cfg.pin_d0 = cfg::kPinB0;
      cfg.pin_d1 = cfg::kPinB1;
      cfg.pin_d2 = cfg::kPinB2;
      cfg.pin_d3 = cfg::kPinB3;
      cfg.pin_d4 = cfg::kPinB4;
      cfg.pin_d5 = cfg::kPinG0;
      cfg.pin_d6 = cfg::kPinG1;
      cfg.pin_d7 = cfg::kPinG2;
      cfg.pin_d8 = cfg::kPinG3;
      cfg.pin_d9 = cfg::kPinG4;
      cfg.pin_d10 = cfg::kPinG5;
      cfg.pin_d11 = cfg::kPinR0;
      cfg.pin_d12 = cfg::kPinR1;
      cfg.pin_d13 = cfg::kPinR2;
      cfg.pin_d14 = cfg::kPinR3;
      cfg.pin_d15 = cfg::kPinR4;

      cfg.pin_henable = cfg::kPinDe;
      cfg.pin_vsync = cfg::kPinVsync;
      cfg.pin_hsync = cfg::kPinHsync;
      cfg.pin_pclk = cfg::kPinPclk;

      cfg.freq_write = cfg::kRgbClockHz;
      cfg.hsync_polarity = 0;
      cfg.hsync_front_porch = 30;
      cfg.hsync_pulse_width = 6;
      cfg.hsync_back_porch = 30;
      cfg.vsync_polarity = 0;
      cfg.vsync_front_porch = 20;
      cfg.vsync_pulse_width = 40;
      cfg.vsync_back_porch = 20;
      cfg.pclk_idle_high = 1;

      bus_.config(cfg);
      panel_.setBus(&bus_);
    }

    setPanel(&panel_);
  }

 private:
  lgfx::Bus_RGB bus_;
  WavesharePanel panel_;
};

WaveshareDisplay gfx;

uint32_t sceneStartMs = 0;
int currentScene = 0;
uint32_t lastHeartbeatMs = 0;
bool sceneDirty = true;

uint16_t rainbow(uint16_t phase) {
  const uint8_t section = (phase / 43) % 6;
  const uint8_t offset = (phase % 43) * 6;

  switch (section) {
    case 0:
      return gfx.color565(255, offset, 0);
    case 1:
      return gfx.color565(255 - offset, 255, 0);
    case 2:
      return gfx.color565(0, 255, offset);
    case 3:
      return gfx.color565(0, 255 - offset, 255);
    case 4:
      return gfx.color565(offset, 0, 255);
    default:
      return gfx.color565(255, 0, 255 - offset);
  }
}

void drawHeader(const char *title, const char *subtitle, uint16_t accent) {
  gfx.fillRoundRect(14, 14, cfg::kScreenWidth - 28, 118, 20, gfx.color565(10, 14, 26));
  gfx.drawRoundRect(14, 14, cfg::kScreenWidth - 28, 118, 20, accent);
  gfx.setTextColor(TFT_WHITE, gfx.color565(10, 14, 26));
  gfx.setTextDatum(top_left);
  gfx.setFont(&fonts::Font4);
  gfx.drawString(title, 28, 28);
  gfx.setFont(&fonts::Font2);
  gfx.setTextColor(gfx.color565(185, 196, 214), gfx.color565(10, 14, 26));
  gfx.drawString(subtitle, 28, 74);
}

void drawBootHint() {
  gfx.setFont(&fonts::Font2);
  gfx.setTextDatum(bottom_center);
  gfx.setTextColor(gfx.color565(205, 214, 230), TFT_BLACK);
  gfx.drawString("Auto cycling display test", cfg::kScreenWidth / 2, cfg::kScreenHeight - 14);
}

void drawColorScene() {
  gfx.fillScreen(TFT_BLACK);
  drawHeader("RGB Panel Demo", "Waveshare ESP32-S3-LCD-3.16", gfx.color565(0, 190, 255));

  const int barY = 170;
  const int barHeight = 88;
  const int barWidth = (cfg::kScreenWidth - 40) / 3;
  const uint16_t colors[3] = {TFT_RED, TFT_GREEN, TFT_BLUE};
  const char *labels[3] = {"RED", "GREEN", "BLUE"};

  gfx.setFont(&fonts::Font4);
  gfx.setTextDatum(middle_center);
  for (int i = 0; i < 3; ++i) {
    const int x = 14 + i * (barWidth + 6);
    gfx.fillRoundRect(x, barY, barWidth, barHeight, 14, colors[i]);
    gfx.setTextColor(TFT_WHITE, colors[i]);
    gfx.drawString(labels[i], x + barWidth / 2, barY + barHeight / 2);
  }

  gfx.setTextColor(gfx.color565(235, 240, 255), TFT_BLACK);
  gfx.setFont(&fonts::Font2);
  gfx.setTextDatum(top_left);
  gfx.drawString("Gradient sweep", 18, 292);

  for (int y = 0; y < 380; ++y) {
    const uint16_t c = rainbow(y & 0xFF);
    gfx.drawFastHLine(18, 324 + y, cfg::kScreenWidth - 36, c);
  }

  const int indicatorX = cfg::kScreenWidth / 2;
  gfx.fillCircle(indicatorX, 514, 14, TFT_WHITE);
  gfx.drawCircle(indicatorX, 514, 22, gfx.color565(0, 0, 0));

  drawBootHint();
}

void drawGeometryScene() {
  gfx.fillScreen(gfx.color565(9, 10, 16));
  drawHeader("Geometry Test", "Lines, circles, arcs and fill rate", gfx.color565(255, 172, 32));

  const int centerX = cfg::kScreenWidth / 2;
  const int centerY = 365;
  for (int i = 0; i < 36; ++i) {
    const float angle = (i * 10.0f) * DEG_TO_RAD;
    const int x = centerX + cosf(angle) * 120.0f;
    const int y = centerY + sinf(angle) * 120.0f;
    gfx.drawLine(centerX, centerY, x, y, rainbow(i * 7));
  }

  for (int r = 120; r > 20; r -= 20) {
    gfx.drawCircle(centerX, centerY, r, rainbow(r * 5));
  }

  const int orbitX = centerX + 88;
  const int orbitY = centerY;
  gfx.fillCircle(orbitX, orbitY, 24, gfx.color565(255, 236, 92));
  gfx.drawCircle(orbitX, orbitY, 32, TFT_WHITE);

  gfx.fillRoundRect(26, 560, cfg::kScreenWidth - 52, 170, 18, gfx.color565(22, 26, 38));
  gfx.drawRoundRect(26, 560, cfg::kScreenWidth - 52, 170, 18, gfx.color565(255, 172, 32));
  gfx.setTextDatum(top_left);
  gfx.setFont(&fonts::Font2);
  gfx.setTextColor(gfx.color565(224, 231, 255), gfx.color565(22, 26, 38));
  gfx.drawString("Rotation / primitive sanity check", 42, 582);
  gfx.drawString("If colors look wrong, swap RGB order", 42, 614);
  gfx.drawString("If tearing appears, reduce RGB clock", 42, 646);
  gfx.drawString("Current RGB clock: 18 MHz", 42, 678);

  drawBootHint();
}

void drawSystemScene() {
  gfx.fillScreen(gfx.color565(6, 11, 8));
  drawHeader("System Stats", "Runtime information and panel sanity", gfx.color565(48, 214, 120));

  const uint32_t freeHeap = ESP.getFreeHeap();
  const uint32_t freePsram = ESP.getFreePsram();
  const float uptime = millis() / 1000.0f;

  const int meterX = 24;
  const int meterY = 182;
  const int meterW = cfg::kScreenWidth - 48;
  const int meterH = 28;
  const int fillW = (meterW - 4) * 3 / 4;

  gfx.setFont(&fonts::Font2);
  gfx.setTextDatum(top_left);
  gfx.setTextColor(gfx.color565(228, 242, 234), TFT_BLACK);
  gfx.drawString("Animated fill test", meterX, meterY - 28);
  gfx.drawRoundRect(meterX, meterY, meterW, meterH, 14, gfx.color565(98, 122, 108));
  gfx.fillRoundRect(meterX + 2, meterY + 2, fillW, meterH - 4, 12, gfx.color565(48, 214, 120));

  gfx.fillRoundRect(20, 248, cfg::kScreenWidth - 40, 286, 18, gfx.color565(18, 26, 20));
  gfx.drawRoundRect(20, 248, cfg::kScreenWidth - 40, 286, 18, gfx.color565(48, 214, 120));
  gfx.setTextColor(TFT_WHITE, gfx.color565(18, 26, 20));
  gfx.setFont(&fonts::Font4);
  gfx.drawString("Heap", 36, 272);
  gfx.setFont(&fonts::Font2);
  gfx.drawString(String(freeHeap / 1024) + " KB free", 36, 320);
  gfx.drawString(String(freePsram / 1024) + " KB PSRAM free", 36, 356);
  gfx.drawString(String(uptime, 1) + " s uptime", 36, 392);
  gfx.drawString("Static render test", 36, 428);
  gfx.drawString("Auto-cycle every 6 seconds", 36, 464);

  for (int i = 0; i < 5; ++i) {
    gfx.fillCircle(58 + i * 52, 600, 18, rainbow(i * 24));
    gfx.drawCircle(58 + i * 52, 600, 24, TFT_WHITE);
  }

  drawBootHint();
}

void drawScene() {
  switch (currentScene) {
    case 0:
      drawColorScene();
      break;
    case 1:
      drawGeometryScene();
      break;
    default:
      drawSystemScene();
      break;
  }
}

void nextScene() {
  currentScene = (currentScene + 1) % 3;
  sceneStartMs = millis();
  sceneDirty = true;
}

}  // namespace

void setup() {
  Serial.begin(115200);
  delay(1000);  // wait for USB CDC enumeration

  DBG_INFO("Waveshare ESP32-S3-LCD-3.16 %s", FIRMWARE_VERSION);
  DBG_INFO("Starting panel init...");

  initBacklight(cfg::kBacklightDefaultBrightness);
  setBacklightBrightness(cfg::kBacklightDefaultBrightness);
  delay(20);

  gfx.init();
  gfx.setRotation(0);
  gfx.setColorDepth(16);

  DBG_INFO("Panel init complete");
  DBG_INFO("Heap free: %u bytes, PSRAM free: %u bytes", ESP.getFreeHeap(), ESP.getFreePsram());

  sceneStartMs = millis();
  lastHeartbeatMs = millis();
  sceneDirty = true;
  drawScene();
  sceneDirty = false;
}

void loop() {
  if (millis() - sceneStartMs > 6000) {
    nextScene();
  }

  if (millis() - lastHeartbeatMs > 1000) {
    DBG_VERBOSE("scene=%d uptime=%lu heap=%u psram=%u",
                currentScene,
                static_cast<unsigned long>(millis()),
                ESP.getFreeHeap(),
                ESP.getFreePsram());
    lastHeartbeatMs = millis();
  }

  if (sceneDirty) {
    drawScene();
    sceneDirty = false;
  }

  delay(20);
}
