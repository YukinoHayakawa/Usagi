#pragma once

namespace usagi
{
enum class GpuAttachmentLoadOp
{
    LOAD,
    CLEAR,
    UNDEFINED,
};

enum class GpuAttachmentStoreOp
{
    STORE,
    UNDEFINED,
};
}
