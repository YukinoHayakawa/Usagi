#pragma once

#include <Usagi/Core/Math.hpp>

namespace usagi
{
enum class GpuFilter
{
    NEAREST,
    LINEAR,
};

enum class GpuSamplerAddressMode
{
    REPEAT,
    MIRRORED_REPEAT,
    CLAMP_TO_EDGE,
    CLAMP_TO_BORDER,
    MIRROR_CLAMP_TO_EDGE,
};

// todo anisotropic filtering
// todo mipmapping
struct GpuSamplerCreateInfo
{
    GpuFilter mag_filter;
    GpuFilter min_filter;
    GpuSamplerAddressMode addressing_mode_u;
    GpuSamplerAddressMode addressing_mode_v;
    Color4f boarder_color;
};
}
