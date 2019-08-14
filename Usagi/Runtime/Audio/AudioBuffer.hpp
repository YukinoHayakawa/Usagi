#pragma once

#include <vector>

namespace usagi
{
struct AudioBuffer
{
    std::uint32_t sample_rate = 0;
    std::size_t frames = 0;
    double time = 0;
    // each vector represents one channel.
    // the size of vector = amount of channels
    std::vector<std::vector<float>> channels;
};
}
