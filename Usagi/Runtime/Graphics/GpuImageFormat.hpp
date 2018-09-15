#pragma once

#include <cstdint>

#include "Enum/GpuBufferFormat.hpp"

namespace usagi
{
struct GpuImageFormat
{
    GpuBufferFormat format;
    std::uint16_t sample_count;
};
}
