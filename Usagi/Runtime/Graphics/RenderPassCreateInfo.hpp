#pragma once

#include <vector>

#include <Usagi/Core/Math.hpp>

#include "Enum/GpuBufferFormat.hpp"
#include "Enum/GpuImageLayout.hpp"

namespace usagi
{
class GpuImage;

enum class GpuAttachmentLoadOp
{
    LOAD,
    CLEAR,
    UNDEFINED
};

enum class GpuAttachmentStoreOp
{
    STORE,
    UNDEFINED
};

struct GpuAttachmentUsage
{
    const GpuBufferFormat format;
    const std::uint16_t sample_count;

    /**
    * \brief The layout before entering any the render pass.
    */
    const GpuImageLayout initial_layout;

    /**
    * \brief The layout used during the render pass.
    */
    GpuImageLayout layout = GpuImageLayout::COLOR_ATTACHMENT;

    /**
    * \brief The layout after the render pass.
    */
    const GpuImageLayout final_layout;

    const GpuAttachmentLoadOp load_op;
    const GpuAttachmentStoreOp store_op;

    Color4f clear_color;

    GpuAttachmentUsage(
        GpuBufferFormat format,
        std::uint16_t sample_count,
        GpuImageLayout initial_layout,
        GpuImageLayout final_layout,
        GpuAttachmentLoadOp load_op,
        GpuAttachmentStoreOp store_op)
        : format(format)
        , sample_count(sample_count)
        , initial_layout(initial_layout)
        , final_layout(final_layout)
        , load_op(load_op)
        , store_op(store_op)
    {
    }
};

struct RenderPassCreateInfo
{
    // attachment indices must be sequential.
    std::vector<GpuAttachmentUsage> attachment_usages;
};
}
