#include "ImageRenderTargetSource.hpp"

#include <Usagi/Runtime/Graphics/GpuDevice.hpp>
#include <Usagi/Runtime/Graphics/GpuImage.hpp>

usagi::ImageRenderTargetSource::ImageRenderTargetSource(
    GpuDevice *gpu,
    GpuImageCreateInfo create_info)
    : mGpu(gpu)
    , mCreateInfo(std::move(create_info))
{
    createImage();
}

usagi::GpuAttachmentUsage & usagi::ImageRenderTargetSource::usage(
    GpuAttachmentUsage &u)
{
    GpuImageLayout layout;
    switch(mCreateInfo.usage)
    {
        case GpuImageUsage::COLOR_ATTACHMENT:
            layout = GpuImageLayout::COLOR_ATTACHMENT;
            break;
        case GpuImageUsage::DEPTH_STENCIL_ATTACHMENT:
            layout = GpuImageLayout::DEPTH_STENCIL_ATTACHMENT;
            break;
        default: throw std::runtime_error("Unsupported image usage");;
    }
    u.format = mImage->format();
    if(u.initial_layout == GpuImageLayout::AUTO)
        u.initial_layout = layout;
    if(u.layout == GpuImageLayout::AUTO)
        u.layout = layout;
    if(u.final_layout == GpuImageLayout::AUTO)
        u.final_layout = layout;
    return u;
}

std::shared_ptr<usagi::GpuImageView>
usagi::ImageRenderTargetSource::view() const
{
    return mImage->baseView();
}

void usagi::ImageRenderTargetSource::createImage()
{
    mImage = mGpu->createImage(mCreateInfo);
}

void usagi::ImageRenderTargetSource::resize(const Vector2u32 &size)
{
    mCreateInfo.size = size;
    createImage();
}
