#pragma once

#include <Usagi/Graphics/RenderTarget/RenderTargetSource.hpp>
#include <Usagi/Runtime/Graphics/GpuImageCreateInfo.hpp>

namespace usagi
{
class GpuDevice;

class ImageRenderTargetSource : public RenderTargetSource
{
    GpuDevice *mGpu = nullptr;
    GpuImageCreateInfo mCreateInfo;
    std::shared_ptr<GpuImage> mImage;

    void createImage();

public:
    ImageRenderTargetSource(GpuDevice *gpu, GpuImageCreateInfo create_info);

    GpuAttachmentUsage & usage(GpuAttachmentUsage &u) override;
    std::shared_ptr<GpuImageView> view() const override;
    void resize(const Vector2u32 &size) override;
};
}
