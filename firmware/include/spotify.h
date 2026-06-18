#pragma once

#include <string>
#include <functional>
#include <memory>

struct SpotifyTrack {
    std::string title;
    std::string artist;
    std::string album;
    std::string art_url;
    int duration_ms = 0;
    int position_ms = 0;
    bool is_playing = false;
};

class SpotifyController {
public:
    using TrackCallback = std::function<void(const SpotifyTrack&)>;

    SpotifyController();
    ~SpotifyController();

    bool connect();
    bool is_connected() const;

    void play();
    void pause();
    void next();
    void previous();
    void seek(int position_ms);
    void set_volume(int percent);

    SpotifyTrack current_track() const;
    void set_track_callback(TrackCallback cb);

    void poll();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
