#include "ui/ui_manager.h"
#include "config.h"
#include <cstdio>

static const char* menu_items[] = {
    "Music",
    "Artists",
    "Albums",
    "Songs",
    "Now Playing",
    "Settings",
};
static const int menu_item_count = 6;

UIManager::UIManager(AudioEngine& audio, MusicLibrary& library, SpotifyController& spotify)
    : audio_(audio), library_(library), spotify_(spotify) {}

UIManager::~UIManager() = default;

void UIManager::init() {
    create_main_menu();
    create_now_playing();
    navigate_to(Screen::MainMenu);
}

void UIManager::create_main_menu() {
    scr_main_menu_ = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(scr_main_menu_, lv_color_black(), 0);

    // Title bar
    lv_obj_t* title = lv_label_create(scr_main_menu_);
    lv_label_set_text(title, "iPod");
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 4);

    // Menu list
    lv_obj_t* list = lv_list_create(scr_main_menu_);
    lv_obj_set_size(list, DISPLAY_WIDTH, DISPLAY_HEIGHT - 30);
    lv_obj_align(list, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(list, lv_color_black(), 0);

    for (int i = 0; i < menu_item_count; i++) {
        lv_obj_t* btn = lv_list_add_button(list, nullptr, menu_items[i]);
        lv_obj_set_style_bg_color(btn, lv_color_black(), 0);
        lv_obj_set_style_text_color(btn, lv_color_white(), 0);
        lv_obj_set_style_bg_color(btn, lv_color_make(0, 0x78, 0xFF), LV_STATE_FOCUSED);
    }
}

void UIManager::create_now_playing() {
    scr_now_playing_ = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(scr_now_playing_, lv_color_black(), 0);

    // Album art placeholder
    img_album_art_ = lv_obj_create(scr_now_playing_);
    lv_obj_set_size(img_album_art_, 140, 140);
    lv_obj_align(img_album_art_, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_set_style_bg_color(img_album_art_, lv_color_make(0x33, 0x33, 0x33), 0);

    // Track title
    lbl_track_title_ = lv_label_create(scr_now_playing_);
    lv_label_set_text(lbl_track_title_, "No Track");
    lv_obj_set_style_text_color(lbl_track_title_, lv_color_white(), 0);
    lv_obj_align(lbl_track_title_, LV_ALIGN_CENTER, 0, 40);
    lv_label_set_long_mode(lbl_track_title_, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(lbl_track_title_, DISPLAY_WIDTH - 20);

    // Artist
    lbl_artist_ = lv_label_create(scr_now_playing_);
    lv_label_set_text(lbl_artist_, "");
    lv_obj_set_style_text_color(lbl_artist_, lv_color_make(0xAA, 0xAA, 0xAA), 0);
    lv_obj_align(lbl_artist_, LV_ALIGN_CENTER, 0, 60);

    // Progress bar
    bar_progress_ = lv_bar_create(scr_now_playing_);
    lv_obj_set_size(bar_progress_, DISPLAY_WIDTH - 40, 6);
    lv_obj_align(bar_progress_, LV_ALIGN_BOTTOM_MID, 0, -30);
    lv_bar_set_range(bar_progress_, 0, 100);

    // Time label
    lbl_time_ = lv_label_create(scr_now_playing_);
    lv_label_set_text(lbl_time_, "0:00 / 0:00");
    lv_obj_set_style_text_color(lbl_time_, lv_color_make(0xAA, 0xAA, 0xAA), 0);
    lv_obj_align(lbl_time_, LV_ALIGN_BOTTOM_MID, 0, -10);
}

void UIManager::navigate_to(Screen screen) {
    if (stack_depth_ < 8) {
        screen_stack_[stack_depth_++] = current_screen_;
    }
    current_screen_ = screen;
    selected_index_ = 0;

    switch (screen) {
        case Screen::MainMenu:
            lv_screen_load(scr_main_menu_);
            break;
        case Screen::NowPlaying:
            lv_screen_load(scr_now_playing_);
            break;
        default:
            break;
    }
}

void UIManager::go_back() {
    if (stack_depth_ > 0) {
        navigate_to(screen_stack_[--stack_depth_]);
        stack_depth_--; // undo the push from navigate_to
    }
}

void UIManager::handle_input(InputEvent event) {
    switch (event) {
        case InputEvent::RotateCW:
            update_selection(1);
            break;
        case InputEvent::RotateCCW:
            update_selection(-1);
            break;
        case InputEvent::EncoderPress:
            if (current_screen_ == Screen::MainMenu) {
                switch (selected_index_) {
                    case 4: navigate_to(Screen::NowPlaying); break;
                    case 5: navigate_to(Screen::Settings); break;
                    default: break;
                }
            }
            break;
        case InputEvent::PlayPausePress:
            if (audio_.state() == PlaybackState::Playing)
                audio_.pause();
            else
                audio_.resume();
            break;
        case InputEvent::BackPress:
        case InputEvent::MenuPress:
            go_back();
            break;
        default:
            break;
    }
}

void UIManager::update_selection(int delta) {
    if (current_screen_ == Screen::MainMenu) {
        selected_index_ = (selected_index_ + delta + menu_item_count) % menu_item_count;
    }
}

void UIManager::update_playback_state(PlaybackState state) {
    // TODO: update play/pause icon on now playing screen
}

void UIManager::update_progress(int position_sec, int duration_sec) {
    if (!bar_progress_ || !lbl_time_) return;

    int pct = duration_sec > 0 ? (position_sec * 100 / duration_sec) : 0;
    lv_bar_set_value(bar_progress_, pct, LV_ANIM_ON);

    char buf[32];
    snprintf(buf, sizeof(buf), "%d:%02d / %d:%02d",
             position_sec / 60, position_sec % 60,
             duration_sec / 60, duration_sec % 60);
    lv_label_set_text(lbl_time_, buf);
}

void UIManager::create_list_screen(const char* title, const std::vector<std::string>& items) {
    // TODO: generic list view for Artists/Albums/Songs
}
