#pragma once

#include <Usagi/Color/Color.hpp>

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
    GpuFilter mag_filter = GpuFilter::LINEAR;
    GpuFilter min_filter = GpuFilter::LINEAR;
    GpuSamplerAddressMode addressing_mode_u = GpuSamplerAddressMode::REPEAT;
    GpuSamplerAddressMode addressing_mode_v = GpuSamplerAddressMode::REPEAT;
    Color4f boarder_color = Color4f::Zero();
};
}
