# ownPod

An open-source, DIY portable music player inspired by the classic iPod. Built on a Raspberry Pi Zero 2W with a custom HAT PCB, featuring local MP3/FLAC playback and Spotify Connect streaming.

![Status](https://img.shields.io/badge/status-work%20in%20progress-yellow)
![License](https://img.shields.io/badge/license-MIT-blue)

## Features

- **Classic iPod UI** — scrolling menus, now playing screen, and cover flow rendered with LVGL on a 2" IPS display
- **Spotify Connect** — appears as a Spotify speaker on your network via librespot
- **Local playback** — MP3, FLAC, WAV, OGG, M4A, AAC from the SD card
- **Click wheel feel** — rotary encoder with acceleration + tactile navigation buttons
- **Portable** — LiPo battery with USB-C charging
- **HiFi audio** — PCM5102A I2S DAC with 3.5mm headphone output
- **Fully open** — firmware, PCB, and case designs all included

## Hardware

| Component | Part | Notes |
|-----------|------|-------|
| SBC | Raspberry Pi Zero 2W | WiFi + Bluetooth built-in |
| DAC | PCM5102A (TSSOP-20) | 32-bit, 384kHz I2S stereo DAC |
| Display | 2" IPS ST7789 (240x320) | SPI interface |
| Input | EC11 rotary encoder + 3 tactile buttons | Menu, Back, Play/Pause |
| Power | LiPo + TP4056 charger + MT3608 boost | USB-C charging, 5V regulated output |
| Audio | 3.5mm TRS headphone jack | DC-coupled output with blocking caps |

Full schematic, BOM, and pin mapping in [`hardware/kicad/CIRCUIT_REFERENCE.md`](hardware/kicad/CIRCUIT_REFERENCE.md).

## Project Structure

```
ownPod/
├── firmware/           C++ application (CMake)
│   ├── src/
│   │   ├── main.cpp            Entry point + event loop
│   │   ├── ui/                 LVGL screens (menu, now playing)
│   │   ├── audio/              libmpv playback engine
│   │   ├── input/              Rotary encoder + button driver (libgpiod)
│   │   ├── display/            ST7789 SPI display driver
│   │   ├── library/            Music file scanner
│   │   └── spotify/            librespot D-Bus (MPRIS2) controller
│   ├── include/                Headers + config
│   └── CMakeLists.txt
├── hardware/           KiCad 8 project
│   └── kicad/
│       ├── ipod-hat.kicad_sch      Schematic
│       ├── ipod-hat.kicad_pcb      PCB layout (RPi HAT form factor)
│       ├── ipod-hat.kicad_sym      Custom symbol library
│       └── CIRCUIT_REFERENCE.md    BOM + wiring reference
├── case/               3D-printable enclosure
│   └── ipod-case.scad              OpenSCAD parametric design
└── scripts/
    └── setup-pi.sh                 One-shot Pi setup script
```

## Getting Started

### Prerequisites

- Raspberry Pi Zero 2W with Raspberry Pi OS Lite (64-bit)
- The assembled ownPod HAT (or breadboard prototype)
- A LiPo battery (3.7V, 1000–2000mAh recommended)

### 1. Set up the Pi

```bash
git clone https://github.com/lori28167/myownmp3player.git
cd myownmp3player
./scripts/setup-pi.sh
sudo reboot
```

This installs all dependencies, enables SPI/I2S, builds librespot, and sets it up as a systemd service.

### 2. Build the firmware

```bash
# Clone LVGL into the lib directory
git clone --depth 1 --branch v9.2.0 https://github.com/lvgl/lvgl.git firmware/lib/lvgl

# Build
cmake -B build -S firmware
cmake --build build -j4

# Run
./build/ownpod-player
```

### 3. Build the case

Open `case/ipod-case.scad` in [OpenSCAD](https://openscad.org/). Render and export the front and back halves as separate STL files. Print with PLA or PETG at 0.2mm layer height.

### 4. Open the schematic

Open `hardware/kicad/ipod-hat.kicad_pro` in [KiCad 8](https://www.kicad.org/). Add the custom symbol library (`ipod-hat.kicad_sym`) via Preferences → Manage Symbol Libraries. See `CIRCUIT_REFERENCE.md` for the full BOM and wiring guide.

## How Spotify Connect Works

ownPod runs [librespot](https://github.com/librespot-org/librespot) as a background daemon. When connected to WiFi, it appears as **"MyiPod"** in the Spotify app's device list. The firmware controls librespot over D-Bus using the standard MPRIS2 interface — play, pause, skip, seek, volume, and track metadata all work from the device's physical controls.

## Pin Mapping

| Function | GPIO (BCM) | Interface |
|----------|-----------|-----------|
| Display SDA | 10 | SPI0 MOSI |
| Display SCK | 11 | SPI0 SCLK |
| Display CS | 8 | SPI0 CE0 |
| Display DC | 25 | Digital |
| Display RST | 24 | Digital |
| Display BL | 12 | PWM0 |
| DAC DIN | 21 | I2S PCM |
| DAC BCLK | 18 | I2S PCM |
| DAC LRCK | 19 | I2S PCM |
| Encoder A | 17 | Digital |
| Encoder B | 27 | Digital |
| Encoder Btn | 22 | Digital |
| Menu | 5 | Digital |
| Back | 6 | Digital |
| Play/Pause | 13 | Digital |

## Contributing

This is a personal project but contributions are welcome. Open an issue or PR if you have improvements, especially for:

- PCB layout optimization
- UI screens (cover flow, settings, search)
- Power management (sleep/wake, battery gauge)
- Bluetooth audio sink support

## License

MIT
