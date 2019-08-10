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

using AudioOutputCallback = std::function<AudioStreamStatus(
    void *output_buffer,
    std::size_t frames
)>;
}
