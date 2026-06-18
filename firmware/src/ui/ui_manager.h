#pragma once

#include "audio.h"
#include "library.h"
#include "spotify.h"
#include "input.h"
#include <lvgl.h>

enum class Screen {
    MainMenu,
    Artists,
    Albums,
    Songs,
    NowPlaying,
    Settings,
};

class UIManager {
public:
    UIManager(AudioEngine& audio, MusicLibrary& library, SpotifyController& spotify);
    ~UIManager();

    void init();
    void handle_input(InputEvent event);
    void update_playback_state(PlaybackState state);
    void update_progress(int position_sec, int duration_sec);

    void navigate_to(Screen screen);
    void go_back();

private:
    void create_main_menu();
    void create_now_playing();
    void create_list_screen(const char* title, const std::vector<std::string>& items);
    void update_selection(int delta);

    AudioEngine& audio_;
    MusicLibrary& library_;
    SpotifyController& spotify_;

    Screen current_screen_ = Screen::MainMenu;
    Screen screen_stack_[8];
    int stack_depth_ = 0;

    int selected_index_ = 0;

    // LVGL objects
    lv_obj_t* scr_main_menu_ = nullptr;
    lv_obj_t* scr_now_playing_ = nullptr;
    lv_obj_t* scr_list_ = nullptr;

    lv_obj_t* lbl_track_title_ = nullptr;
    lv_obj_t* lbl_artist_ = nullptr;
    lv_obj_t* bar_progress_ = nullptr;
    lv_obj_t* img_album_art_ = nullptr;
    lv_obj_t* lbl_time_ = nullptr;
};
