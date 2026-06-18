#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>

struct Track {
    std::string path;
    std::string title;
    std::string artist;
    std::string album;
    int duration_sec = 0;
    std::string art_path;
};

enum class PlaybackState {
    Stopped,
    Playing,
    Paused,
};

class AudioEngine {
public:
    using StateCallback = std::function<void(PlaybackState)>;
    using ProgressCallback = std::function<void(int position_sec, int duration_sec)>;

    AudioEngine();
    ~AudioEngine();

    void play(const std::string& path);
    void pause();
    void resume();
    void stop();
    void seek(int seconds);
    void set_volume(int percent);
    int get_volume() const;

    PlaybackState state() const;
    int position_sec() const;

    void set_state_callback(StateCallback cb);
    void set_progress_callback(ProgressCallback cb);

    void tick();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
