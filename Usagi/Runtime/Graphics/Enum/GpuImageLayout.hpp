#pragma once

namespace usagi
{
enum class GpuImageLayout
{
    UNDEFINED,
    PRESENT,
    TRANSFER_SRC,
    TRANSFER_DST,
    COLOR_ATTACHMENT,
    DEPTH_STENCIL_ATTACHMENT,
    SHADER_READ_ONLY,
    PREINITIALIZED,
    AUTO,
};
}
