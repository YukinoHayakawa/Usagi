#pragma once

#include <functional>

namespace usagi
{
enum class AudioStreamStatus
{
    CONTINUE = 0,
    STOP = 1,
    ABORT = 2,
};

// todo pass float*?
using AudioOutputCallback = std::function<AudioStreamStatus(
    std::uint8_t *output_buffer,
    std::size_t frames
)>;
}
