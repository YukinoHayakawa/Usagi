﻿#pragma once

#include <Usagi/Math/Matrix.hpp>

#include "Enum/GpuBufferFormat.hpp"
#include "Enum/GpuImageUsage.hpp"

namespace usagi
{
struct GpuImageCreateInfo
{
    GpuBufferFormat format;
    Vector2u32 size;
    std::uint16_t mip_levels = 1;
    std::uint16_t sample_count = 1;
    GpuImageUsage usage;
};
}
