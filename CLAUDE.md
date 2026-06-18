# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Custom iPod-like portable music player built on a Raspberry Pi Zero 2W. Plays local MP3/FLAC files and supports Spotify Connect via librespot. The UI replicates the classic iPod interface (cover flow, scrolling lists, click wheel navigation) rendered on a 2" IPS SPI display using LVGL.

## Architecture

```
┌─────────────────────────────────────────────┐
│              RPi Zero 2W                    │
│                                             │
│  ┌──────────┐  ┌──────────┐  ┌───────────┐ │
│  │ UI/LVGL  │  │  Audio   │  │  Spotify  │ │
│  │ (SPI     │  │  Engine  │  │  (librespot│ │
│  │  ST7789) │  │ (MPD/    │  │   daemon) │ │
│  │          │  │  libmpv) │  │           │ │
│  └────┬─────┘  └────┬─────┘  └─────┬─────┘ │
│       │              │              │       │
│  ┌────┴─────┐  ┌────┴─────┐  ┌─────┴─────┐ │
│  │  Input   │  │  I2S     │  │   WiFi    │ │
│  │  (GPIO   │  │  (PCM    │  │   (built  │ │
│  │  encoder │  │  5102A   │  │    in)    │ │
│  │  +btns)  │  │   DAC)   │  │           │ │
│  └──────────┘  └──────────┘  └───────────┘ │
└─────────────────────────────────────────────┘
         ↕               ↕
    Custom HAT PCB (DAC + controls + display + power)
```

### Firmware (C/C++)

- **Build system**: CMake, cross-compiled with `aarch64-linux-gnu` toolchain or compiled natively on the Pi
- **UI**: LVGL v9 rendering to Linux framebuffer (`/dev/fb0`) via SPI-driven ST7789
- **Audio**: libmpv or MPD client library for local playback; librespot runs as a separate daemon for Spotify
- **Input**: GPIO rotary encoder + buttons via libgpiod
- **Source layout**:
  - `firmware/src/main.cpp` — entry point, event loop
  - `firmware/src/ui/` — LVGL screens (now playing, menu, cover flow, settings)
  - `firmware/src/audio/` — playback engine, playlist management
  - `firmware/src/input/` — rotary encoder and button driver
  - `firmware/src/spotify/` — librespot D-Bus (MPRIS) control: play/pause/next/prev, metadata, album art
  - `firmware/include/` — shared headers (config.h has pin mappings, lv_conf.h has LVGL settings)
  - `firmware/assets/` — fonts, icons, bitmaps

### Hardware (KiCad)

- `hardware/kicad/` — schematic + PCB for the Pi HAT carrying:
  - PCM5102A I2S DAC
  - 3.5mm headphone jack
  - Rotary encoder + navigation buttons
  - ST7789 display connector (FPC)
  - LiPo charging (TP4056) + 5V boost (TPS61090 or similar)
  - Power switch + battery connector

## Build Commands

```bash
# Configure (from repo root)
cmake -B build -S firmware -DCMAKE_TOOLCHAIN_FILE=firmware/cmake/rpi-zero2w-toolchain.cmake

# Build
cmake --build build -j$(nproc)

# Build natively on the Pi (no toolchain file needed)
cmake -B build -S firmware && cmake --build build -j4

# Deploy to Pi over SSH
scp build/ipod-player pi@<PI_IP>:/home/pi/player/

# Run on Pi
ssh pi@<PI_IP> '/home/pi/player/ipod-player'

# Format code
clang-format -i firmware/src/**/*.cpp firmware/include/**/*.h

# Run librespot (Spotify Connect daemon)
librespot --name "MyiPod" --backend alsa --device hw:0,0 --bitrate 320
```

## Key Dependencies

- **LVGL v9** — UI framework, configured for 240x320 framebuffer
- **libmpv** — audio decoding and playback
- **libgpiod** — GPIO access for buttons/encoder (not the deprecated sysfs interface)
- **librespot** — Spotify Connect (Rust binary, runs as a daemon)
- **libdbus-1** — D-Bus IPC for controlling librespot via MPRIS2
- **stb_image** — album art decoding

## Case

- `case/ipod-case.scad` — OpenSCAD parametric case design, split into front/back halves
- Print settings: PLA/PETG, 0.2mm layers, 20% infill
- Render in OpenSCAD, export each half as STL for slicing

## Hardware Pin Mapping (active reference for firmware and PCB)

| Function        | GPIO (BCM) | Interface |
|----------------|------------|-----------|
| Display SDA    | GPIO 10    | SPI0 MOSI |
| Display SCK    | GPIO 11    | SPI0 SCLK |
| Display CS     | GPIO 8     | SPI0 CE0  |
| Display DC     | GPIO 25    | Digital   |
| Display RST    | GPIO 24    | Digital   |
| Display BL     | GPIO 12    | PWM0      |
| DAC DIN        | GPIO 21    | I2S PCM   |
| DAC BCLK       | GPIO 18    | I2S PCM   |
| DAC LRCK       | GPIO 19    | I2S PCM   |
| Encoder A      | GPIO 17    | Digital   |
| Encoder B      | GPIO 27    | Digital   |
| Encoder Button | GPIO 22    | Digital   |
| Btn Menu       | GPIO 5     | Digital   |
| Btn Back       | GPIO 6     | Digital   |
| Btn Play/Pause | GPIO 13    | Digital   |
| Battery ADC    | GPIO 26    | SPI1 (MCP3008) |

**Note**: Display backlight uses GPIO 12 (PWM0 channel 0, alt func 4) to avoid conflict with I2S BCLK on GPIO 18.
