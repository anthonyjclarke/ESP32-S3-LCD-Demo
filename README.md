# ESP32-S3-LCD-3.16 Demo

<!-- Update version badge when FIRMWARE_VERSION changes in include/config.h -->
![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)
![Platform](https://img.shields.io/badge/platform-ESP32--S3-green.svg)
![PlatformIO](https://img.shields.io/badge/PlatformIO-6.x-orange.svg)
![Board](https://img.shields.io/badge/Waveshare-ESP32--S3--LCD--3.16-yellow.svg)
![License](https://img.shields.io/badge/license-MIT-lightgrey.svg)
![Status](https://img.shields.io/badge/status-stable-green.svg)

---

PlatformIO project for the Waveshare `ESP32-S3-LCD-3.16` board. Drives the ST7701 RGB parallel panel via LovyanGFX with a three-scene cycling display demo.

---

## Hardware

| Feature      | Detail                                      |
|:-------------|:--------------------------------------------|
| MCU          | ESP32-S3R8 · dual-core 240 MHz              |
| Flash        | 16 MB                                       |
| PSRAM        | 8 MB octal (OPI)                            |
| Display      | 3.16" · 320 × 820 · ST7701 · RGB parallel   |
| Backlight    | GPIO6 · inverted LEDC PWM                   |
| Serial       | USB CDC (native USB port)                   |

---

## Pin Assignment

### LCD command interface (sideband SPI)

| Signal | GPIO |
|:-------|:-----|
| CS     | 0    |
| MOSI   | 1    |
| SCK    | 2    |
| RST    | 16   |

### RGB data bus

| Channel | GPIOs              |
|:--------|:-------------------|
| Red     | 17, 46, 3, 8, 18   |
| Green   | 14, 13, 12, 11, 10, 9 |
| Blue    | 21, 5, 45, 48, 47  |

### RGB timing

| Signal | GPIO |
|:-------|:-----|
| HSYNC  | 38   |
| VSYNC  | 39   |
| DE     | 40   |
| PCLK   | 41   |

RGB clock: 18 MHz.

---

## PlatformIO Configuration

Key settings required for this board (not covered by the generic `esp32-s3-devkitm-1` definition):

- `board_build.arduino.memory_type = qio_opi` — required for 8 MB OPI PSRAM
- `board_build.partitions = default_16MB.csv` — 16 MB flash layout
- `-DBOARD_HAS_PSRAM` — enables `ps_malloc()` and PSRAM APIs
- `-DARDUINO_USB_CDC_ON_BOOT=1` — routes `Serial` over USB CDC

---

## Display Notes

The ST7701 is not a simple SPI TFT. It uses an RGB parallel pixel data bus with a separate sideband command interface. The backlight behaves as active-low at the firmware level:

- lower LEDC duty = brighter output
- `255` brightness maps to `0` duty (fully on)
- Driving GPIO6 HIGH causes the backlight to go dark after reset

---

## Build

```bash
pio run
```

## Upload

```bash
pio run -t upload
```

If upload fails, enter bootloader manually:

1. Close any serial monitor on `/dev/cu.usbmodem1301`
2. Hold `BOOT`, press and release `RST`, then release `BOOT`
3. Retry upload
4. Press `RST` once after upload if the board does not auto-run

## Serial Monitor

```bash
pio device monitor -b 115200 -p /dev/cu.usbmodem1301
```

Expected boot output:

```
[INFO]  Waveshare ESP32-S3-LCD-3.16 1.0.0
[INFO]  Starting panel init...
[INFO]  Panel init complete
[INFO]  Heap free: ... bytes, PSRAM free: ... bytes
```

Heartbeat lines (`[VERBOSE]`) appear every second at `DEBUG_LEVEL=4`.

---

## Demo Behaviour

Three static scenes cycle every 6 seconds:

1. **RGB / gradient** — solid colour bars and a full-width rainbow sweep
2. **Geometry** — radial lines, concentric circles, and a diagnostic text block
3. **System stats** — heap, PSRAM, and uptime readout

Static scene rendering was chosen over continuous redraw to eliminate visible flicker.

---

## Known Bring-Up Findings

- The board can show a valid image briefly during reset even when backlight control is wrong — if that happens the RGB path is likely working and the fault is backlight polarity.
- Inverted LEDC PWM on GPIO6 is the confirmed working solution for this hardware.
- `TFT_eSPI` cannot drive this panel — LovyanGFX with `Panel_ST7701_Base` is required.
