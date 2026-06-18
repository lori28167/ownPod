#include "input.h"
#include "config.h"
#include <gpiod.h>
#include <chrono>

struct InputDriver::Impl {
    Callback callback;
    struct gpiod_chip* chip = nullptr;
    struct gpiod_line* enc_a = nullptr;
    struct gpiod_line* enc_b = nullptr;
    struct gpiod_line* enc_btn = nullptr;
    struct gpiod_line* btn_menu = nullptr;
    struct gpiod_line* btn_back = nullptr;
    struct gpiod_line* btn_play = nullptr;

    int last_enc_a = 0;
    int last_enc_b = 0;

    std::chrono::steady_clock::time_point enc_btn_down_time;
    bool enc_btn_held = false;
};

InputDriver::InputDriver() : impl_(std::make_unique<Impl>()) {
    impl_->chip = gpiod_chip_open_by_name("gpiochip0");
    if (!impl_->chip) return;

    auto request_input = [&](int pin) -> struct gpiod_line* {
        auto* line = gpiod_chip_get_line(impl_->chip, pin);
        if (line) {
            gpiod_line_request_input_flags(line, "ipod-player",
                GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_UP);
        }
        return line;
    };

    impl_->enc_a = request_input(PIN_ENC_A);
    impl_->enc_b = request_input(PIN_ENC_B);
    impl_->enc_btn = request_input(PIN_ENC_BTN);
    impl_->btn_menu = request_input(PIN_BTN_MENU);
    impl_->btn_back = request_input(PIN_BTN_BACK);
    impl_->btn_play = request_input(PIN_BTN_PLAY);

    if (impl_->enc_a) impl_->last_enc_a = gpiod_line_get_value(impl_->enc_a);
    if (impl_->enc_b) impl_->last_enc_b = gpiod_line_get_value(impl_->enc_b);
}

InputDriver::~InputDriver() {
    if (impl_->chip) gpiod_chip_close(impl_->chip);
}

void InputDriver::set_callback(Callback cb) {
    impl_->callback = std::move(cb);
}

void InputDriver::poll() {
    if (!impl_->callback || !impl_->chip) return;

    // Rotary encoder (Gray code)
    if (impl_->enc_a && impl_->enc_b) {
        int a = gpiod_line_get_value(impl_->enc_a);
        int b = gpiod_line_get_value(impl_->enc_b);

        if (a != impl_->last_enc_a) {
            if (a != b) {
                impl_->callback(InputEvent::RotateCW);
            } else {
                impl_->callback(InputEvent::RotateCCW);
            }
        }
        impl_->last_enc_a = a;
        impl_->last_enc_b = b;
    }

    // Encoder button with long press detection
    if (impl_->enc_btn) {
        int val = gpiod_line_get_value(impl_->enc_btn);
        if (val == 0 && !impl_->enc_btn_held) {
            impl_->enc_btn_held = true;
            impl_->enc_btn_down_time = std::chrono::steady_clock::now();
            impl_->callback(InputEvent::EncoderPress);
        } else if (val == 1 && impl_->enc_btn_held) {
            impl_->enc_btn_held = false;
            auto held_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - impl_->enc_btn_down_time).count();
            if (held_ms > 800) {
                impl_->callback(InputEvent::EncoderLongPress);
            }
            impl_->callback(InputEvent::EncoderRelease);
        }
    }

    // Simple buttons (active low)
    auto check_button = [&](struct gpiod_line* line, InputEvent event) {
        if (line && gpiod_line_get_value(line) == 0) {
            impl_->callback(event);
        }
    };

    check_button(impl_->btn_menu, InputEvent::MenuPress);
    check_button(impl_->btn_back, InputEvent::BackPress);
    check_button(impl_->btn_play, InputEvent::PlayPausePress);
}
