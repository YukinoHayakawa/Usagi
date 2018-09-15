#include "VulkanGpuImage.hpp"

#include <Usagi/Runtime/Graphics/GpuImageViewCreateInfo.hpp>

#include "VulkanEnumTranslation.hpp"
#include "VulkanGpuImageView.hpp"

using namespace usagi::vulkan;

vk::ImageAspectFlags usagi::VulkanGpuImage::getAspectsFromFormat() const
{
    vk::ImageAspectFlags aspects;
    switch(translate(mFormat.format)) {
        case vk::Format::eD16Unorm:
        case vk::Format::eD32Sfloat:
            aspects = vk::ImageAspectFlagBits::eDepth;
            break;
        case vk::Format::eD16UnormS8Uint:
        case vk::Format::eD24UnormS8Uint:
        case vk::Format::eD32SfloatS8Uint:
            aspects =
                vk::ImageAspectFlagBits::eDepth |
                vk::ImageAspectFlagBits::eStencil;
            break;
        default:
            aspects = vk::ImageAspectFlagBits::eColor;
    }
    return aspects;
}

void usagi::VulkanGpuImage::createBaseView()
{
    vk::ImageViewCreateInfo info;
    info.setImage(image());
    info.setViewType(vk::ImageViewType::e2D);
    info.setFormat(translate(mFormat.format));
    info.setComponents(vk::ComponentMapping { });
    vk::ImageSubresourceRange subresource_range;
    subresource_range.setAspectMask(getAspectsFromFormat());
    subresource_range.setBaseArrayLayer(0);
    subresource_range.setLayerCount(1);
    subresource_range.setBaseMipLevel(0);
    subresource_range.setLevelCount(1);
    info.setSubresourceRange(subresource_range);

    mBaseView = std::make_shared<VulkanGpuImageView>(
        this, mDevice.createImageViewUnique(info));
}

usagi::VulkanGpuImage::VulkanGpuImage(
    GpuImageFormat format,
    const Vector2u32 &size,
    vk::Device vk_device)
    : GpuImage(format, size)
    , mDevice(vk_device)
{
}

std::shared_ptr<usagi::GpuImageView> usagi::VulkanGpuImage::baseView()
{
    return mBaseView;
}

std::shared_ptr<usagi::GpuImageView> usagi::VulkanGpuImage::createView(
    const GpuImageViewCreateInfo &info)
{
    vk::ImageViewCreateInfo vk_info;
    vk_info.setImage(image());
    vk_info.setViewType(vk::ImageViewType::e2D);
    vk_info.setFormat(translate(mFormat.format));
    vk_info.components.r = translate(info.components.r);
    vk_info.components.g = translate(info.components.g);
    vk_info.components.b = translate(info.components.b);
    vk_info.components.a = translate(info.components.a);
    vk::ImageSubresourceRange subresource_range;
    subresource_range.setAspectMask(getAspectsFromFormat());
    subresource_range.setBaseArrayLayer(0);
    subresource_range.setLayerCount(1);
    subresource_range.setBaseMipLevel(0);
    subresource_range.setLevelCount(1);
    vk_info.setSubresourceRange(subresource_range);

    return std::make_shared<VulkanGpuImageView>(
        this, mDevice.createImageViewUnique(vk_info));
}
