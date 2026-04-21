# Changelog

## [1.0.0] 2026-04-21

### Changed
- Status badge updated to `stable`; merged to `main` as confirmed working baseline

## [1.0.0] 2026-04-10

### Added
- Initial LovyanGFX bring-up for Waveshare ESP32-S3-LCD-3.16
- ST7701 RGB parallel panel driver via `WaveshareDisplay` class extending `Panel_ST7701_Base`
- Inverted LEDC PWM backlight control on GPIO6
- Three-scene cycling demo: RGB/gradient, geometry, system stats
- `debug.h` leveled debug macro system
- `FIRMWARE_VERSION` in `config.h`
- Project scaffolding: `LICENSE`, `CHANGELOG.md`, `CLAUDE.md`, `.gitignore`
