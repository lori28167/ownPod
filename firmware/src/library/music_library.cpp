#include "library.h"
#include <filesystem>
#include <algorithm>
#include <thread>
#include <atomic>

namespace fs = std::filesystem;

static bool is_audio_file(const fs::path& path) {
    auto ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".mp3" || ext == ".flac" || ext == ".wav"
        || ext == ".ogg" || ext == ".m4a" || ext == ".aac";
}

struct MusicLibrary::Impl {
    std::vector<Track> tracks;
    std::vector<Album> albums;
    std::vector<Artist> artists;
    std::atomic<bool> scanning{false};
};

MusicLibrary::MusicLibrary() : impl_(std::make_unique<Impl>()) {}

void MusicLibrary::scan(const std::string& root_path) {
    if (impl_->scanning) return;
    impl_->scanning = true;

    // Scan in background thread
    std::thread([this, root_path]() {
        impl_->tracks.clear();
        impl_->albums.clear();
        impl_->artists.clear();

        std::map<std::string, std::map<std::string, std::vector<Track>>> artist_album_map;

        try {
            for (auto& entry : fs::recursive_directory_iterator(root_path)) {
                if (!entry.is_regular_file()) continue;
                if (!is_audio_file(entry.path())) continue;

                Track t;
                t.path = entry.path().string();
                t.title = entry.path().stem().string();

                // Derive artist/album from directory structure: Artist/Album/Track.mp3
                auto rel = fs::relative(entry.path(), root_path);
                auto parts = std::vector<std::string>();
                for (auto& p : rel) parts.push_back(p.string());

                if (parts.size() >= 3) {
                    t.artist = parts[0];
                    t.album = parts[1];
                } else if (parts.size() == 2) {
                    t.artist = parts[0];
                    t.album = "Unknown Album";
                }

                impl_->tracks.push_back(t);
                artist_album_map[t.artist][t.album].push_back(t);
            }
        } catch (...) {}

        // Build album and artist lists
        for (auto& [artist_name, albums] : artist_album_map) {
            Artist artist;
            artist.name = artist_name;
            for (auto& [album_name, tracks] : albums) {
                Album album;
                album.name = album_name;
                album.artist = artist_name;
                album.tracks = tracks;
                artist.albums.push_back(album);
                impl_->albums.push_back(album);
            }
            impl_->artists.push_back(artist);
        }

        impl_->scanning = false;
    }).detach();
}

bool MusicLibrary::is_scanning() const {
    return impl_->scanning;
}

const std::vector<Track>& MusicLibrary::all_tracks() const {
    return impl_->tracks;
}

const std::vector<Album>& MusicLibrary::all_albums() const {
    return impl_->albums;
}

const std::vector<Artist>& MusicLibrary::all_artists() const {
    return impl_->artists;
}

std::vector<Track> MusicLibrary::search(const std::string& query) const {
    std::vector<Track> results;
    std::string q = query;
    std::transform(q.begin(), q.end(), q.begin(), ::tolower);

    for (auto& t : impl_->tracks) {
        std::string title = t.title;
        std::transform(title.begin(), title.end(), title.begin(), ::tolower);
        if (title.find(q) != std::string::npos) {
            results.push_back(t);
        }
    }
    return results;
}
