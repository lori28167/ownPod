#!/bin/bash
# Setup script for Raspberry Pi Zero 2W
# Run this on the Pi after flashing Raspberry Pi OS Lite (64-bit)

set -e

echo "=== Updating system ==="
sudo apt update && sudo apt upgrade -y

echo "=== Installing build dependencies ==="
sudo apt install -y \
    build-essential cmake pkg-config \
    libmpv-dev \
    libgpiod-dev \
    libasound2-dev \
    libdbus-1-dev \
    git

echo "=== Enabling SPI and I2S ==="
# Enable SPI
if ! grep -q "^dtparam=spi=on" /boot/firmware/config.txt; then
    echo "dtparam=spi=on" | sudo tee -a /boot/firmware/config.txt
fi

# Enable I2S audio overlay for PCM5102A DAC
if ! grep -q "^dtoverlay=hifiberry-dac" /boot/firmware/config.txt; then
    echo "dtoverlay=hifiberry-dac" | sudo tee -a /boot/firmware/config.txt
fi

# Disable onboard audio (conflicts with I2S)
sudo sed -i 's/^dtparam=audio=on/#dtparam=audio=on/' /boot/firmware/config.txt

echo "=== Installing librespot (Spotify Connect) ==="
if ! command -v librespot &> /dev/null; then
    curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
    source "$HOME/.cargo/env"
    git clone https://github.com/librespot-org/librespot.git /tmp/librespot
    cd /tmp/librespot
    cargo build --release --no-default-features --features alsa-backend
    sudo cp target/release/librespot /usr/local/bin/
    cd -
fi

echo "=== Creating librespot systemd service ==="
sudo tee /etc/systemd/system/librespot.service > /dev/null <<'EOF'
[Unit]
Description=Librespot Spotify Connect
After=network-online.target sound.target
Wants=network-online.target

[Service]
ExecStart=/usr/local/bin/librespot --name "MyiPod" --backend alsa --device hw:0,0 --bitrate 320 --initial-volume 80
Restart=always
RestartSec=5
User=pi

[Install]
WantedBy=multi-user.target
EOF

sudo systemctl daemon-reload
sudo systemctl enable librespot

echo "=== Cloning LVGL ==="
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
LVGL_DIR="$SCRIPT_DIR/../firmware/lib/lvgl"
if [ ! -d "$LVGL_DIR" ]; then
    git clone --depth 1 --branch v9.2.0 https://github.com/lvgl/lvgl.git "$LVGL_DIR"
fi

echo "=== Setup complete! Reboot required for SPI/I2S changes ==="
echo "Run: sudo reboot"
