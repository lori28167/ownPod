# iPod HAT Circuit Reference

Use this alongside the KiCad schematic. Net labels connect matching signals
between sections — open the schematic in KiCad and wire pins to their labels.

## Net Label Connections

These net labels define all inter-section connections:

### I2S Bus (RPi → DAC)
| Net Label | RPi Pin (BCM) | DAC Pin |
|-----------|---------------|---------|
| I2S_BCK   | GPIO 18 (pin 12) | PCM5102A pin 6 (BCK) |
| I2S_DIN   | GPIO 21 (pin 40) | PCM5102A pin 7 (DIN) |
| I2S_LRCK  | GPIO 19 (pin 35) | PCM5102A pin 8 (LRCK) |

### SPI Bus (RPi → Display)
| Net Label | RPi Pin (BCM) | Display Pin |
|-----------|---------------|-------------|
| SPI_MOSI  | GPIO 10 (pin 19) | J5 pin 4 (SDA) |
| SPI_SCLK  | GPIO 11 (pin 23) | J5 pin 3 (SCK) |
| SPI_CE0   | GPIO 8 (pin 24)  | J5 pin 7 (CS)  |

### Display Control (RPi → Display)
| Net Label | RPi Pin (BCM) | Display Pin |
|-----------|---------------|-------------|
| DISP_DC   | GPIO 25 (pin 22) | J5 pin 5 (DC)  |
| DISP_RST  | GPIO 24 (pin 18) | J5 pin 6 (RST) |
| DISP_BL   | GPIO 12 (pin 32) | J5 pin 8 (BL)  |

### Input (RPi → Encoder/Buttons)
| Net Label | RPi Pin (BCM) | Component |
|-----------|---------------|-----------|
| ENC_A     | GPIO 17 (pin 11) | SW2 pin A |
| ENC_B     | GPIO 27 (pin 13) | SW2 pin B |
| ENC_BTN   | GPIO 22 (pin 15) | SW2 pin S1 |
| BTN_MENU  | GPIO 5 (pin 29)  | SW3 pin 1 |
| BTN_BACK  | GPIO 6 (pin 31)  | SW4 pin 1 |
| BTN_PLAY  | GPIO 13 (pin 33) | SW5 pin 1 |

### Power
| Net Label | From | To |
|-----------|------|----|
| VBUS      | USB-C J3 VBUS | TP4056 U2 VCC (pin 4) |
| VBAT      | TP4056 U2 BAT (pin 7) | Battery J4, Power switch SW1 |
| VBAT_SW   | Power switch SW1 | MT3608 U3 IN (pin 1), EN (pin 3) |
| +5V       | MT3608 output | RPi pin 2/4 via header |
| +3V3      | RPi pin 1/17 | DAC XSMT, DAC AVDD, Display VCC |

## Bill of Materials

| Ref | Value | Package | Description |
|-----|-------|---------|-------------|
| U1  | PCM5102A | TSSOP-20 | I2S stereo DAC |
| U2  | TP4056 | SOP-8 | LiPo charge controller |
| U3  | MT3608 | SOT-23-6 | Step-up boost converter |
| J1  | 2x20 Header | 2.54mm | RPi GPIO (socket, not pin) |
| J2  | 3.5mm Jack | CUI SJ-3523 | Headphone output |
| J3  | USB-C | GCT USB4125 | Charging input |
| J4  | JST-PH 2-pin | B2B-PH-K | LiPo battery connector |
| J5  | 1x8 Header | 2.54mm | Display connector |
| SW1 | Slide switch | 1x2 pin | Power on/off |
| SW2 | EC11 Encoder | Alps EC11E | Rotary encoder + push |
| SW3 | Tactile | TL3342 | Menu button |
| SW4 | Tactile | TL3342 | Back button |
| SW5 | Tactile | TL3342 | Play/Pause button |
| D1  | Red LED | 0402 | Charging indicator |
| D2  | Green LED | 0402 | Standby indicator |
| D3  | SS14 | SMA | Schottky diode (boost) |
| L1  | 4.7uH | NR3015 | Boost inductor |
| R1  | 5.1k | 0402 | USB-C CC1 pull-down |
| R2  | 5.1k | 0402 | USB-C CC2 pull-down |
| R3  | 2k | 0402 | TP4056 PROG (580mA charge) |
| R4  | 1k | 0402 | Charging LED resistor |
| R5  | 1k | 0402 | Standby LED resistor |
| R6  | 10k | 0402 | TEMP pin (NTC or fixed to GND) |
| R7  | 100k | 0402 | Boost FB divider top |
| R8  | 27k | 0402 | Boost FB divider bottom |
| C1  | 100nF | 0402 | PCM5102A LDOO decoupling |
| C2  | 100nF | 0402 | PCM5102A DVDD decoupling |
| C3  | 10uF | 0805 | PCM5102A AVDD decoupling |
| C4  | 2.2uF | 0402 | Charge pump (CAPP-CPVDD) |
| C5  | 1uF | 0402 | VNEG decoupling |
| C6  | 2.2uF | 0402 | Audio L DC blocking |
| C7  | 2.2uF | 0402 | Audio R DC blocking |
| C8  | 22uF | 0805 | Boost input cap |
| C9  | 22uF | 0805 | Boost output cap |
| C10 | 100nF | 0402 | Encoder A debounce |
| C11 | 100nF | 0402 | Encoder B debounce |
| C12 | 100nF | 0402 | Menu btn debounce |
| C13 | 100nF | 0402 | Back btn debounce |
| C14 | 100nF | 0402 | Play btn debounce |

## PCM5102A Configuration Pins

| Pin | Connection | Effect |
|-----|-----------|--------|
| FLT (1) | GND | Normal latency filter |
| DEMP (2) | GND | De-emphasis disabled |
| XSMT (3) | +3V3 | Soft mute OFF (always playing) |
| FMT (4) | GND | I2S format (standard) |
| SCK (5) | GND | Use internal PLL clock |

## MT3608 Boost Converter Design

- Input: 3.0V–4.2V (LiPo)
- Output: 5.0V
- Feedback: Vout = Vref × (1 + R7/R8) = 0.6V × (1 + 100k/27k) ≈ 2.82V ... 

**NOTE**: The MT3608 Vref is actually 0.6V and Vout = 0.6 × (1 + R_top/R_bot).
For 5V output: R7/R8 = (5.0/0.6) - 1 = 7.33 → use 100k/13.7k or 150k/20k.
**Adjust R7=150k, R8=20k for accurate 5V output.** Update in schematic.

## RPi Zero 2W 40-Pin Header Reference

```
           3V3  (1) (2)  5V
    GPIO2/SDA  (3) (4)  5V
    GPIO3/SCL  (5) (6)  GND
        GPIO4  (7) (8)  GPIO14/TXD
          GND  (9) (10) GPIO15/RXD
  ENC_A/GPIO17 (11)(12) GPIO18/I2S_BCK
  ENC_B/GPIO27 (13)(14) GND
ENC_BTN/GPIO22 (15)(16) GPIO23
          3V3 (17)(18) GPIO24/DISP_RST
SPI_MOSI/GPIO10(19)(20) GND
       GPIO9  (21)(22) GPIO25/DISP_DC
SPI_SCLK/GPIO11(23)(24) GPIO8/SPI_CE0
          GND (25)(26) GPIO7
       GPIO0  (27)(28) GPIO1
BTN_MENU/GPIO5 (29)(30) GND
 BTN_BACK/GPIO6(31)(32) GPIO12/DISP_BL
BTN_PLAY/GPIO13(33)(34) GND
I2S_LRCK/GPIO19(35)(36) GPIO16
       GPIO26 (37)(38) GPIO20
          GND (39)(40) GPIO21/I2S_DIN
```
