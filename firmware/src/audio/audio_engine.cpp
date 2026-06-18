#include "audio.h"
#include <mpv/client.h>
#include <cstring>

struct AudioEngine::Impl {
    mpv_handle* mpv = nullptr;
    PlaybackState state = PlaybackState::Stopped;
    int volume = 80;
    int position = 0;
    int duration = 0;

    StateCallback state_cb;
    ProgressCallback progress_cb;
};

AudioEngine::AudioEngine() : impl_(std::make_unique<Impl>()) {
    impl_->mpv = mpv_create();
    if (!impl_->mpv) return;

    mpv_set_option_string(impl_->mpv, "vo", "null");
    mpv_set_option_string(impl_->mpv, "ao", "alsa");
    mpv_set_option_string(impl_->mpv, "audio-device", "alsa/hw:0,0");
    mpv_initialize(impl_->mpv);
}

AudioEngine::~AudioEngine() {
    if (impl_->mpv) mpv_terminate_destroy(impl_->mpv);
}

void AudioEngine::play(const std::string& path) {
    if (!impl_->mpv) return;
    const char* cmd[] = {"loadfile", path.c_str(), nullptr};
    mpv_command(impl_->mpv, cmd);
    impl_->state = PlaybackState::Playing;
    if (impl_->state_cb) impl_->state_cb(impl_->state);
}

void AudioEngine::pause() {
    if (!impl_->mpv) return;
    int flag = 1;
    mpv_set_property(impl_->mpv, "pause", MPV_FORMAT_FLAG, &flag);
    impl_->state = PlaybackState::Paused;
    if (impl_->state_cb) impl_->state_cb(impl_->state);
}

void AudioEngine::resume() {
    if (!impl_->mpv) return;
    int flag = 0;
    mpv_set_property(impl_->mpv, "pause", MPV_FORMAT_FLAG, &flag);
    impl_->state = PlaybackState::Playing;
    if (impl_->state_cb) impl_->state_cb(impl_->state);
}

void AudioEngine::stop() {
    if (!impl_->mpv) return;
    const char* cmd[] = {"stop", nullptr};
    mpv_command(impl_->mpv, cmd);
    impl_->state = PlaybackState::Stopped;
    if (impl_->state_cb) impl_->state_cb(impl_->state);
}

void AudioEngine::seek(int seconds) {
    if (!impl_->mpv) return;
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", seconds);
    const char* cmd[] = {"seek", buf, "absolute", nullptr};
    mpv_command(impl_->mpv, cmd);
}

void AudioEngine::set_volume(int percent) {
    if (!impl_->mpv) return;
    impl_->volume = percent;
    double vol = percent;
    mpv_set_property(impl_->mpv, "volume", MPV_FORMAT_DOUBLE, &vol);
}

int AudioEngine::get_volume() const {
    return impl_->volume;
}

PlaybackState AudioEngine::state() const {
    return impl_->state;
}

int AudioEngine::position_sec() const {
    return impl_->position;
}

void AudioEngine::set_state_callback(StateCallback cb) {
    impl_->state_cb = std::move(cb);
}

void AudioEngine::set_progress_callback(ProgressCallback cb) {
    impl_->progress_cb = std::move(cb);
}

void AudioEngine::tick() {
    if (!impl_->mpv || impl_->state == PlaybackState::Stopped) return;

    double pos = 0, dur = 0;
    mpv_get_property(impl_->mpv, "time-pos", MPV_FORMAT_DOUBLE, &pos);
    mpv_get_property(impl_->mpv, "duration", MPV_FORMAT_DOUBLE, &dur);

    impl_->position = static_cast<int>(pos);
    impl_->duration = static_cast<int>(dur);

    if (impl_->progress_cb) {
        impl_->progress_cb(impl_->position, impl_->duration);
    }

    // Check for end of file
    mpv_event* event = mpv_wait_event(impl_->mpv, 0);
    if (event->event_id == MPV_EVENT_END_FILE) {
        impl_->state = PlaybackState::Stopped;
        if (impl_->state_cb) impl_->state_cb(impl_->state);
    }
}
