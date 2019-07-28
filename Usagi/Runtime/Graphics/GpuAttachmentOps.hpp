#pragma once

#include <Usagi/Color/Color.hpp>
#include <Usagi/Runtime/Graphics/Enum/GpuAttachmentOp.hpp>

namespace usagi
{
struct GpuAttachmentOps
{
    GpuAttachmentLoadOp load_op = GpuAttachmentLoadOp::LOAD;
    GpuAttachmentStoreOp store_op = GpuAttachmentStoreOp::STORE;
    Color4f clear_color = Color4f::Zero();
};
}
