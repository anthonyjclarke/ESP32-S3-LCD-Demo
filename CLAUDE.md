# CLAUDE.md — ESP32-S3-LCD-3.16

## Target Hardware

**Waveshare ESP32-S3-LCD-3.16**
- MCU: ESP32-S3R8 · dual-core 240 MHz
- Flash: 16 MB
- PSRAM: 8 MB octal OPI (mapped via `qio_opi`)
- Display: 3.16" · 320 × 820 · ST7701 · RGB parallel bus (not SPI)
- Serial: USB CDC on native USB port (`-DARDUINO_USB_CDC_ON_BOOT=1`)

## What This Project Does

LovyanGFX bring-up and display demo for the Waveshare ESP32-S3-LCD-3.16. Drives the ST7701 RGB parallel panel with a three-scene cycling demo (colour/gradient, geometry, system stats). Intended as a working hardware baseline and test platform.

## Non-Obvious Pin Assignments

- **Backlight**: GPIO6, inverted LEDC PWM — duty 0 = full brightness, 255 = off. This is opposite to typical active-high setups.
- **LCD command interface**: GPIO0 (CS), GPIO1 (MOSI), GPIO2 (SCK), GPIO16 (RST) — this is a sideband SPI interface for ST7701 init commands, separate from the RGB pixel data bus.
- **RGB bus**: 16-bit parallel across GPIOs 3, 5, 8–14, 17–18, 21, 45–48 — see `config.h` for full pin mapping.
- **USB CDC**: Serial routes over native USB, not CP2102. Port is `/dev/cu.usbmodem1301`.

## Library

LovyanGFX (`lovyan03/LovyanGFX`) is required. `TFT_eSPI` cannot drive an ST7701 RGB panel — do not suggest switching.

`WaveshareDisplay` in `src/main.cpp` extends `lgfx::LGFX_Device`. It contains an inner class `WavesharePanel` that extends `lgfx::Panel_ST7701_Base` and overrides `getInitCommands()` with the full vendor init sequence. Both are self-contained in `main.cpp`.

LovyanGFX framebuffer is allocated in PSRAM (`use_psram = 1` in `panel_.config_detail()`). PSRAM must be present and correctly configured — if it is absent, display init will silently fail or produce garbage output.

## Fonts

This project uses LovyanGFX built-in fonts (`fonts::Font2`, `fonts::Font4`). These are accessed directly from the `lgfx::fonts` namespace and do not require loading. The VLW font rule from the global CLAUDE.md applies to CYD/TFT_eSPI projects only and does not apply here.

## Known Hardware Quirks

- Board shows a brief valid image during reset even with incorrect backlight control. This means the RGB path is working; suspect backlight polarity first if the screen goes dark after init.
- PSRAM requires `board_build.arduino.memory_type = qio_opi` — the generic devkitm-1 profile does not set this.
- `default_16MB.csv` is used (not a custom partition table) — the 16 MB flash layout is handled by the PlatformIO built-in file.
- `kScreenHeight = 820` is correct for this panel — it is a tall narrow 320 × 820 display.
- RGB bus data pins are wired in BGR order (d0–d4 = Blue, d5–d10 = Green, d11–d15 = Red). The panel config sets `rgb_order = true` to swap R/B and produce correct colours. If colours look wrong, check this flag first.
- Backlight LEDC uses **channel 1** (`kBacklightPwmChannel`) on **timer 3** (`LEDC_TIMER_3`). Any future LEDC peripheral (e.g. buzzer, additional PWM) must use a different channel and timer.
- `gfx.setRotation(0)` is portrait orientation, which is natural for the tall 320 × 820 aspect ratio.

## Flashing Notes

- Port: `/dev/cu.usbmodem1301`
- If upload fails: hold BOOT → press RST → release BOOT → retry upload → press RST after flash completes
- No special upload flags required beyond what is in `platformio.ini`

## Global Rules That Do Not Apply Here

- CYD / TFT_eSPI rules: this is not a CYD board and does not use TFT_eSPI
- WiFiManager / secrets.h: no WiFi in this project
- Custom partition table: not required — uses `default_16MB.csv`
- XPT2046 touch: no touchscreen on this board
