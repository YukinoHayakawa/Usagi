#pragma once

#include <memory>

#include <Usagi/Math/Matrix.hpp>
#include <Usagi/Runtime/Graphics/RenderPassCreateInfo.hpp>

namespace usagi
{
struct GpuAttachmentOps;
class GpuImageView;

class RenderTargetSource
{
public:
    virtual ~RenderTargetSource() = default;

    /**
     * \brief Fill in format & layout.
     * \param u
     * \return
     */
    virtual GpuAttachmentUsage & usage(GpuAttachmentUsage &u) = 0;

    virtual std::shared_ptr<GpuImageView> view() const = 0;

    /**
     * \brief Original contend not preserved.
     * \param size
     */
    virtual void resize(const Vector2u32 &size) = 0;
};
}
