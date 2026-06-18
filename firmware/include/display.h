#pragma once

#include <cstdint>
#include <memory>

class Display {
public:
    Display();
    ~Display();

    void init();
    void set_backlight(uint8_t brightness);
    void flush_cb(int x1, int y1, int x2, int y2, const uint8_t* color_data);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
