#pragma once

#include <cstdint>

#include <Usagi/Constants/DataFormat.hpp>

namespace usagi
{
struct AudioStreamFormat
{
    std::uint8_t num_channels = 2;
    DataFormat format = DataFormat::INT16;
    bool interleaved = true;
    double sample_rate = 48000;
    // todo endianness?
};
}
