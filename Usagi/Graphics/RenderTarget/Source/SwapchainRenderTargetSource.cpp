#include "SwapchainRenderTargetSource.hpp"

#include <Usagi/Runtime/Graphics/Swapchain.hpp>
#include <Usagi/Runtime/Graphics/GpuImage.hpp>


usagi::SwapchainRenderTargetSource::SwapchainRenderTargetSource(
    Swapchain *swapchain)
    : mSwapchain(swapchain)
{
}

usagi::GpuAttachmentUsage & usagi::SwapchainRenderTargetSource::usage(
    GpuAttachmentUsage &u)
{
    u.format.format = mSwapchain->format();
    // swapchain images only have 1 sample
    u.format.sample_count = 1;
    if(u.initial_layout == GpuImageLayout::AUTO)
        u.initial_layout = GpuImageLayout::COLOR_ATTACHMENT;
    if(u.layout == GpuImageLayout::AUTO)
        u.layout = GpuImageLayout::COLOR_ATTACHMENT;
    if(u.final_layout == GpuImageLayout::AUTO)
        u.final_layout = GpuImageLayout::COLOR_ATTACHMENT;
    return u;
}

std::shared_ptr<usagi::GpuImageView>
usagi::SwapchainRenderTargetSource::view() const
{
    return mSwapchain->currentImage()->baseView();
}

void usagi::SwapchainRenderTargetSource::resize(const Vector2u32 &size)
{
    mSwapchain->resize(size);
}
