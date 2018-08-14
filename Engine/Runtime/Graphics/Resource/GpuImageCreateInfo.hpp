#pragma once

#include <Usagi/Engine/Core/Math.hpp>
#include <Usagi/Engine/Runtime/Graphics/Enum/GpuBufferFormat.hpp>
#include <Usagi/Engine/Runtime/Graphics/Enum/GpuImageUsage.hpp>

namespace usagi
{
struct GpuImageCreateInfo
{
    GpuBufferFormat format;
    Vector2u32 size;
    std::uint32_t mip_levels = 1;
    std::uint32_t sample_count = 1;
    GpuImageUsage usage;
};
}
