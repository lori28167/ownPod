#include "spotify.h"
#include <dbus/dbus.h>
#include <cstring>
#include <cstdio>

static const char* LIBRESPOT_BUS = "org.mpris.MediaPlayer2.librespot";
static const char* MPRIS_PATH = "/org/mpris/MediaPlayer2";
static const char* MPRIS_PLAYER = "org.mpris.MediaPlayer2.Player";
static const char* DBUS_PROPS = "org.freedesktop.DBus.Properties";

struct SpotifyController::Impl {
    DBusConnection* conn = nullptr;
    SpotifyTrack track;
    TrackCallback track_cb;
    bool connected = false;

    DBusMessage* call_method(const char* iface, const char* method) {
        if (!conn) return nullptr;
        DBusMessage* msg = dbus_message_new_method_call(
            LIBRESPOT_BUS, MPRIS_PATH, iface, method);
        if (!msg) return nullptr;

        DBusError err;
        dbus_error_init(&err);
        DBusMessage* reply = dbus_connection_send_with_reply_and_block(
            conn, msg, 1000, &err);
        dbus_message_unref(msg);

        if (dbus_error_is_set(&err)) {
            dbus_error_free(&err);
            return nullptr;
        }
        return reply;
    }

    std::string get_string_property(const char* prop) {
        DBusMessage* msg = dbus_message_new_method_call(
            LIBRESPOT_BUS, MPRIS_PATH, DBUS_PROPS, "Get");
        if (!msg) return "";

        const char* iface = MPRIS_PLAYER;
        dbus_message_append_args(msg,
            DBUS_TYPE_STRING, &iface,
            DBUS_TYPE_STRING, &prop,
            DBUS_TYPE_INVALID);

        DBusError err;
        dbus_error_init(&err);
        DBusMessage* reply = dbus_connection_send_with_reply_and_block(
            conn, msg, 1000, &err);
        dbus_message_unref(msg);

        if (dbus_error_is_set(&err)) {
            dbus_error_free(&err);
            return "";
        }

        DBusMessageIter iter, variant;
        dbus_message_iter_init(reply, &iter);
        dbus_message_iter_recurse(&iter, &variant);

        const char* val = nullptr;
        if (dbus_message_iter_get_arg_type(&variant) == DBUS_TYPE_STRING) {
            dbus_message_iter_get_basic(&variant, &val);
        }

        std::string result = val ? val : "";
        dbus_message_unref(reply);
        return result;
    }

    void update_metadata() {
        DBusMessage* msg = dbus_message_new_method_call(
            LIBRESPOT_BUS, MPRIS_PATH, DBUS_PROPS, "Get");
        if (!msg) return;

        const char* iface = MPRIS_PLAYER;
        const char* prop = "Metadata";
        dbus_message_append_args(msg,
            DBUS_TYPE_STRING, &iface,
            DBUS_TYPE_STRING, &prop,
            DBUS_TYPE_INVALID);

        DBusError err;
        dbus_error_init(&err);
        DBusMessage* reply = dbus_connection_send_with_reply_and_block(
            conn, msg, 1000, &err);
        dbus_message_unref(msg);

        if (dbus_error_is_set(&err)) {
            dbus_error_free(&err);
            return;
        }

        // Parse the metadata variant{dict} — MPRIS metadata is a{sv}
        DBusMessageIter iter, variant, dict;
        dbus_message_iter_init(reply, &iter);
        dbus_message_iter_recurse(&iter, &variant);

        if (dbus_message_iter_get_arg_type(&variant) != DBUS_TYPE_ARRAY) {
            dbus_message_unref(reply);
            return;
        }

        dbus_message_iter_recurse(&variant, &dict);
        while (dbus_message_iter_get_arg_type(&dict) == DBUS_TYPE_DICT_ENTRY) {
            DBusMessageIter entry, val;
            dbus_message_iter_recurse(&dict, &entry);

            const char* key = nullptr;
            dbus_message_iter_get_basic(&entry, &key);
            dbus_message_iter_next(&entry);
            dbus_message_iter_recurse(&entry, &val);

            if (strcmp(key, "xesam:title") == 0) {
                const char* s = nullptr;
                if (dbus_message_iter_get_arg_type(&val) == DBUS_TYPE_STRING)
                    dbus_message_iter_get_basic(&val, &s);
                if (s) track.title = s;
            } else if (strcmp(key, "xesam:album") == 0) {
                const char* s = nullptr;
                if (dbus_message_iter_get_arg_type(&val) == DBUS_TYPE_STRING)
                    dbus_message_iter_get_basic(&val, &s);
                if (s) track.album = s;
            } else if (strcmp(key, "xesam:artist") == 0) {
                // artist is an array of strings, take first
                if (dbus_message_iter_get_arg_type(&val) == DBUS_TYPE_ARRAY) {
                    DBusMessageIter arr;
                    dbus_message_iter_recurse(&val, &arr);
                    const char* s = nullptr;
                    if (dbus_message_iter_get_arg_type(&arr) == DBUS_TYPE_STRING)
                        dbus_message_iter_get_basic(&arr, &s);
                    if (s) track.artist = s;
                }
            } else if (strcmp(key, "mpris:artUrl") == 0) {
                const char* s = nullptr;
                if (dbus_message_iter_get_arg_type(&val) == DBUS_TYPE_STRING)
                    dbus_message_iter_get_basic(&val, &s);
                if (s) track.art_url = s;
            } else if (strcmp(key, "mpris:length") == 0) {
                int64_t len = 0;
                if (dbus_message_iter_get_arg_type(&val) == DBUS_TYPE_INT64)
                    dbus_message_iter_get_basic(&val, &len);
                track.duration_ms = static_cast<int>(len / 1000);
            }

            dbus_message_iter_next(&dict);
        }

        // Playback status
        std::string status = get_string_property("PlaybackStatus");
        track.is_playing = (status == "Playing");

        dbus_message_unref(reply);
    }
};

SpotifyController::SpotifyController() : impl_(std::make_unique<Impl>()) {}

SpotifyController::~SpotifyController() {
    if (impl_->conn) dbus_connection_unref(impl_->conn);
}

bool SpotifyController::connect() {
    DBusError err;
    dbus_error_init(&err);
    impl_->conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err)) {
        dbus_error_free(&err);
        return false;
    }
    impl_->connected = (impl_->conn != nullptr);
    return impl_->connected;
}

bool SpotifyController::is_connected() const {
    return impl_->connected;
}

void SpotifyController::play() {
    auto* reply = impl_->call_method(MPRIS_PLAYER, "Play");
    if (reply) dbus_message_unref(reply);
}

void SpotifyController::pause() {
    auto* reply = impl_->call_method(MPRIS_PLAYER, "Pause");
    if (reply) dbus_message_unref(reply);
}

void SpotifyController::next() {
    auto* reply = impl_->call_method(MPRIS_PLAYER, "Next");
    if (reply) dbus_message_unref(reply);
}

void SpotifyController::previous() {
    auto* reply = impl_->call_method(MPRIS_PLAYER, "Previous");
    if (reply) dbus_message_unref(reply);
}

void SpotifyController::seek(int position_ms) {
    DBusMessage* msg = dbus_message_new_method_call(
        LIBRESPOT_BUS, MPRIS_PATH, MPRIS_PLAYER, "Seek");
    int64_t offset_us = static_cast<int64_t>(position_ms) * 1000;
    dbus_message_append_args(msg, DBUS_TYPE_INT64, &offset_us, DBUS_TYPE_INVALID);

    DBusError err;
    dbus_error_init(&err);
    auto* reply = dbus_connection_send_with_reply_and_block(
        impl_->conn, msg, 1000, &err);
    dbus_message_unref(msg);
    if (reply) dbus_message_unref(reply);
    if (dbus_error_is_set(&err)) dbus_error_free(&err);
}

void SpotifyController::set_volume(int percent) {
    DBusMessage* msg = dbus_message_new_method_call(
        LIBRESPOT_BUS, MPRIS_PATH, DBUS_PROPS, "Set");

    const char* iface = MPRIS_PLAYER;
    const char* prop = "Volume";
    dbus_message_append_args(msg,
        DBUS_TYPE_STRING, &iface,
        DBUS_TYPE_STRING, &prop,
        DBUS_TYPE_INVALID);

    DBusMessageIter iter, variant;
    dbus_message_iter_init_append(msg, &iter);
    dbus_message_iter_open_container(&iter, DBUS_TYPE_VARIANT, "d", &variant);
    double vol = percent / 100.0;
    dbus_message_iter_append_basic(&variant, DBUS_TYPE_DOUBLE, &vol);
    dbus_message_iter_close_container(&iter, &variant);

    DBusError err;
    dbus_error_init(&err);
    auto* reply = dbus_connection_send_with_reply_and_block(
        impl_->conn, msg, 1000, &err);
    dbus_message_unref(msg);
    if (reply) dbus_message_unref(reply);
    if (dbus_error_is_set(&err)) dbus_error_free(&err);
}

SpotifyTrack SpotifyController::current_track() const {
    return impl_->track;
}

void SpotifyController::set_track_callback(TrackCallback cb) {
    impl_->track_cb = std::move(cb);
}

void SpotifyController::poll() {
    if (!impl_->conn) return;

    SpotifyTrack old_track = impl_->track;
    impl_->update_metadata();

    if (impl_->track_cb &&
        (old_track.title != impl_->track.title ||
         old_track.is_playing != impl_->track.is_playing)) {
        impl_->track_cb(impl_->track);
    }
}
