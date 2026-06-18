#include "config.h"
#include "display.h"
#include "input.h"
#include "audio.h"
#include "library.h"
#include "spotify.h"
#include "ui/ui_manager.h"

#include <lvgl.h>
#include <signal.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <atomic>

static std::atomic<bool> running{true};

static void signal_handler(int) {
    running = false;
}

int main() {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    lv_init();

    Display display;
    display.init();

    // Create LVGL display with framebuffer driver
    auto* lv_display = lv_display_create(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    static uint8_t buf1[DISPLAY_WIDTH * DISPLAY_HEIGHT * 2];
    lv_display_set_buffers(lv_display, buf1, nullptr, sizeof(buf1), LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_flush_cb(lv_display, [](lv_display_t* disp, const lv_area_t* area, uint8_t* px_map) {
        // TODO: wire up SPI flush to display
        lv_display_flush_ready(disp);
    });

    InputDriver input;
    AudioEngine audio;
    MusicLibrary library;

    library.scan(MUSIC_ROOT);

    SpotifyController spotify;
    spotify.connect();

    UIManager ui(audio, library, spotify);
    ui.init();

    input.set_callback([&ui](InputEvent event) {
        ui.handle_input(event);
    });

    audio.set_state_callback([&ui](PlaybackState state) {
        ui.update_playback_state(state);
    });

    audio.set_progress_callback([&ui](int pos, int dur) {
        ui.update_progress(pos, dur);
    });

    auto last_tick = std::chrono::steady_clock::now();

    while (running) {
        input.poll();
        audio.tick();
        spotify.poll();

        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_tick).count();
        lv_tick_inc(elapsed);
        last_tick = now;

        lv_timer_handler();

        std::this_thread::sleep_for(std::chrono::milliseconds(UI_TICK_MS));
    }

    return 0;
}
