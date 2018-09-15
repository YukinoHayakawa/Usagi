#pragma once

#include <vector>

#include "Enum/GpuBufferFormat.hpp"
#include "Enum/GpuImageLayout.hpp"
#include "Enum/GpuAttachmentOp.hpp"
#include "GpuImageFormat.hpp"
#include "GpuAttachmentOps.hpp"

namespace usagi
{
class GpuImage;

struct GpuAttachmentUsage
{
    GpuImageFormat format;
    // auto
    /**
    * \brief The layout before entering any the render pass.
    */
    GpuImageLayout initial_layout = GpuImageLayout::AUTO;

    /**
    * \brief The layout used during the render pass.
    */
    GpuImageLayout layout = GpuImageLayout::AUTO;

    /**
    * \brief The layout after the render pass.
    */
    GpuImageLayout final_layout = GpuImageLayout::AUTO;

    GpuAttachmentOps op;
};

struct RenderPassCreateInfo
{
    // attachment indices must be sequential.
    std::vector<GpuAttachmentUsage> attachment_usages;
};
}
