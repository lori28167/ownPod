#pragma once

#include <functional>
#include <memory>

enum class InputEvent {
    RotateCW,
    RotateCCW,
    EncoderPress,
    EncoderRelease,
    EncoderLongPress,
    MenuPress,
    BackPress,
    PlayPausePress,
};

class InputDriver {
public:
    using Callback = std::function<void(InputEvent)>;

    InputDriver();
    ~InputDriver();

    void set_callback(Callback cb);
    void poll();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
