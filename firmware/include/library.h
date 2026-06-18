#pragma once

#include "audio.h"
#include <string>
#include <vector>
#include <map>

struct Album {
    std::string name;
    std::string artist;
    std::string art_path;
    std::vector<Track> tracks;
};

struct Artist {
    std::string name;
    std::vector<Album> albums;
};

class MusicLibrary {
public:
    MusicLibrary();

    void scan(const std::string& root_path);
    bool is_scanning() const;

    const std::vector<Track>& all_tracks() const;
    const std::vector<Album>& all_albums() const;
    const std::vector<Artist>& all_artists() const;

    std::vector<Track> search(const std::string& query) const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
