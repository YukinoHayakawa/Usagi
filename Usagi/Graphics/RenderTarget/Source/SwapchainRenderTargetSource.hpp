#pragma once

#include "../RenderTargetSource.hpp"

namespace usagi
{
class Swapchain;

class SwapchainRenderTargetSource : public RenderTargetSource
{
    Swapchain *mSwapchain = nullptr;

public:
    explicit SwapchainRenderTargetSource(Swapchain *swapchain);

    GpuAttachmentUsage & usage(GpuAttachmentUsage &u) override;
    std::shared_ptr<GpuImageView> view() const override;
    void resize(const Vector2u32 &size) override;
};
}
