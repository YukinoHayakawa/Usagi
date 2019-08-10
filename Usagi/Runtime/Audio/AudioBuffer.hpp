#pragma once

#include <vector>

#include "AudioStreamFormat.hpp"

namespace usagi
{
struct AudioBuffer
{
    AudioStreamFormat format;
    std::vector<float> samples;
    std::size_t num_frames;
};
}
