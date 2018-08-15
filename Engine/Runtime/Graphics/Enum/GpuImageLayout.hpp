#pragma once

namespace usagi
{
enum class GpuImageLayout
{
    UNDEFINED,
    PRESENT,
    TRANSFER_SRC,
    TRANSFER_DST,
    COLOR,
    SHADER_READ_ONLY,
    PREINITIALIZED,
};
}
