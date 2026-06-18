#include "display.h"
#include "config.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <cstring>
#include <gpiod.h>

struct Display::Impl {
    int spi_fd = -1;
    struct gpiod_chip* chip = nullptr;
    struct gpiod_line* dc_line = nullptr;
    struct gpiod_line* rst_line = nullptr;
    struct gpiod_line* bl_line = nullptr;

    void send_command(uint8_t cmd);
    void send_data(const uint8_t* data, size_t len);
    void spi_transfer(const uint8_t* data, size_t len);
};

void Display::Impl::spi_transfer(const uint8_t* data, size_t len) {
    struct spi_ioc_transfer tr{};
    tr.tx_buf = reinterpret_cast<unsigned long>(data);
    tr.len = len;
    tr.speed_hz = DISPLAY_SPI_SPEED_HZ;
    tr.bits_per_word = 8;
    ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
}

void Display::Impl::send_command(uint8_t cmd) {
    gpiod_line_set_value(dc_line, 0);
    spi_transfer(&cmd, 1);
}

void Display::Impl::send_data(const uint8_t* data, size_t len) {
    gpiod_line_set_value(dc_line, 1);
    spi_transfer(data, len);
}

Display::Display() : impl_(std::make_unique<Impl>()) {}

Display::~Display() {
    if (impl_->spi_fd >= 0) close(impl_->spi_fd);
    if (impl_->chip) gpiod_chip_close(impl_->chip);
}

void Display::init() {
    // Open SPI
    impl_->spi_fd = open(DISPLAY_SPI_DEV, O_RDWR);
    if (impl_->spi_fd < 0) return;

    uint32_t mode = SPI_MODE_0;
    uint32_t speed = DISPLAY_SPI_SPEED_HZ;
    uint8_t bits = 8;
    ioctl(impl_->spi_fd, SPI_IOC_WR_MODE, &mode);
    ioctl(impl_->spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    ioctl(impl_->spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits);

    // Setup GPIO
    impl_->chip = gpiod_chip_open_by_name("gpiochip0");
    if (!impl_->chip) return;

    impl_->dc_line = gpiod_chip_get_line(impl_->chip, PIN_DC);
    impl_->rst_line = gpiod_chip_get_line(impl_->chip, PIN_RST);
    impl_->bl_line = gpiod_chip_get_line(impl_->chip, PIN_BL);

    gpiod_line_request_output(impl_->dc_line, "ipod-dc", 0);
    gpiod_line_request_output(impl_->rst_line, "ipod-rst", 1);
    gpiod_line_request_output(impl_->bl_line, "ipod-bl", 1);

    // Hardware reset
    gpiod_line_set_value(impl_->rst_line, 0);
    usleep(10000);
    gpiod_line_set_value(impl_->rst_line, 1);
    usleep(120000);

    // ST7789 init sequence
    impl_->send_command(0x01); // Software reset
    usleep(150000);
    impl_->send_command(0x11); // Sleep out
    usleep(500000);

    impl_->send_command(0x3A); // Color mode
    uint8_t color_mode = 0x55; // 16-bit RGB565
    impl_->send_data(&color_mode, 1);

    impl_->send_command(0x36); // Memory access control
    uint8_t madctl = 0x00;
    impl_->send_data(&madctl, 1);

    impl_->send_command(0x29); // Display on
    usleep(100000);
}

void Display::set_backlight(uint8_t brightness) {
    if (impl_->bl_line) {
        gpiod_line_set_value(impl_->bl_line, brightness > 0 ? 1 : 0);
    }
}

void Display::flush_cb(int x1, int y1, int x2, int y2, const uint8_t* color_data) {
    // Set column address
    impl_->send_command(0x2A);
    uint8_t col_data[] = {
        static_cast<uint8_t>(x1 >> 8), static_cast<uint8_t>(x1 & 0xFF),
        static_cast<uint8_t>(x2 >> 8), static_cast<uint8_t>(x2 & 0xFF)
    };
    impl_->send_data(col_data, 4);

    // Set row address
    impl_->send_command(0x2B);
    uint8_t row_data[] = {
        static_cast<uint8_t>(y1 >> 8), static_cast<uint8_t>(y1 & 0xFF),
        static_cast<uint8_t>(y2 >> 8), static_cast<uint8_t>(y2 & 0xFF)
    };
    impl_->send_data(row_data, 4);

    // Write pixel data
    impl_->send_command(0x2C);
    size_t len = (x2 - x1 + 1) * (y2 - y1 + 1) * 2;
    impl_->send_data(color_data, len);
}
