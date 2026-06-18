#pragma once

// Display (ST7789 over SPI)
#define DISPLAY_WIDTH   240
#define DISPLAY_HEIGHT  320
#define DISPLAY_SPI_DEV "/dev/spidev0.0"
#define DISPLAY_SPI_SPEED_HZ 62500000
#define PIN_DC          25
#define PIN_RST         24
#define PIN_BL          12  // moved from 18 to avoid I2S conflict

// Rotary encoder
#define PIN_ENC_A       17
#define PIN_ENC_B       27
#define PIN_ENC_BTN     22

// Navigation buttons
#define PIN_BTN_MENU    5
#define PIN_BTN_BACK    6
#define PIN_BTN_PLAY    13

// Audio
#define AUDIO_DEVICE    "hw:0,0"

// Paths
#define MUSIC_ROOT      "/home/pi/Music"
#define ALBUM_ART_CACHE "/tmp/ipod-art-cache"

// UI
#define SCROLL_ACCEL_THRESHOLD  5   // encoder ticks before acceleration kicks in
#define SCROLL_ACCEL_FACTOR     3
#define UI_TICK_MS              5   // LVGL tick period
#define BACKLIGHT_TIMEOUT_MS    30000
