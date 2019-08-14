#pragma once

#include <functional>

#include <Usagi/Utility/ArrayView.hpp>

namespace usagi
{
enum class AudioStreamStatus
{
    CONTINUE = 0,
    STOP = 1,
    ABORT = 2,
};

// todo use std::span
using AudioOutputCallback = std::function<AudioStreamStatus(
    const ArrayView<std::byte*> &channels,
    std::size_t frames
)>;
}
