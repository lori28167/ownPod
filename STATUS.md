# ownPod — Project Status

Last updated: 2026-06-18

## Overall: 🟡 Scaffolding Complete — Not Yet Tested

---

### Firmware

| Module | Code | Compiles | Hardware Tested |
|--------|------|----------|-----------------|
| Main loop / event loop | ✅ Done | ❌ Needs LVGL clone | ❌ |
| Display driver (ST7789 SPI) | ✅ Done | ❌ | ❌ |
| Input driver (encoder + buttons) | ✅ Done | ❌ | ❌ |
| Audio engine (libmpv) | ✅ Done | ❌ | ❌ |
| Music library scanner | ✅ Done | ❌ | ❌ |
| Spotify controller (D-Bus/MPRIS2) | ✅ Done | ❌ | ❌ |
| UI — Main menu | ✅ Done | ❌ | ❌ |
| UI — Now playing screen | ✅ Done | ❌ | ❌ |
| UI — Artist/Album/Song lists | ❌ TODO | — | — |
| UI — Cover flow | ❌ TODO | — | — |
| UI — Settings screen | ❌ TODO | — | — |
| UI — Spotify now playing | ❌ TODO | — | — |
| Playlist / queue management | ❌ TODO | — | — |
| Volume control (UI + HW) | ❌ TODO | — | — |
| Power management / sleep | ❌ TODO | — | — |
| Battery level monitoring | ❌ TODO | — | — |

### Hardware

| Item | Status | Notes |
|------|--------|-------|
| Schematic — components placed | ✅ Done | All ICs, passives, connectors in KiCad |
| Schematic — net labels defined | ✅ Done | I2S, SPI, GPIO, power nets all labeled |
| Schematic — wiring in KiCad | ❌ TODO | Pins need manual wiring to labels in GUI |
| Schematic — ERC pass | ❌ TODO | Run after wiring |
| Custom symbol library | ✅ Done | PCM5102A, TP4056, MT3608, DW01A |
| PCB — board outline | ✅ Done | 65x56.5mm RPi HAT form factor |
| PCB — mounting holes | ✅ Done | 4x M2.5, matching RPi Zero 2W |
| PCB — ground planes | ✅ Done | Both layers |
| PCB — footprint import | ❌ TODO | Run "Update PCB from Schematic" |
| PCB — component placement | ❌ TODO | |
| PCB — trace routing | ❌ TODO | |
| PCB — DRC pass | ❌ TODO | |
| PCB — gerber export | ❌ TODO | |
| PCB — order from fab | ❌ TODO | JLCPCB / PCBWay |
| Breadboard prototype | ❌ TODO | Test circuit before ordering PCB |

### Case

| Item | Status | Notes |
|------|--------|-------|
| OpenSCAD parametric design | ✅ Done | Front + back halves |
| Display cutout | ✅ Done | 28x35mm |
| Encoder + button holes | ✅ Done | |
| Port cutouts (3.5mm, USB-C) | ✅ Done | |
| Speaker grille | ✅ Done | |
| PCB standoffs | ✅ Done | M2.5 |
| STL export | ❌ TODO | Render each half separately |
| Test print | ❌ TODO | |
| Fitment check | ❌ TODO | |

### Infrastructure

| Item | Status |
|------|--------|
| Git repo | ✅ github.com/lori28167/ownPod |
| README | ✅ Done |
| LICENSE (MIT) | ✅ Done |
| CLAUDE.md | ✅ Done |
| Pi setup script | ✅ Done |
| CMake build system | ✅ Done |
| Cross-compile toolchain | ✅ Done |
| CI/CD | ❌ TODO |
| .gitignore | ❌ TODO |

---

## Next Steps (in order)

1. Add `.gitignore` for build artifacts
2. Clone LVGL and verify firmware compiles on a Pi
3. Breadboard the PCM5102A DAC circuit and test I2S audio
4. Wire the KiCad schematic and pass ERC
5. Layout and route the PCB
6. Order PCB + parts
7. Test print the case
8. Assemble and bring up
