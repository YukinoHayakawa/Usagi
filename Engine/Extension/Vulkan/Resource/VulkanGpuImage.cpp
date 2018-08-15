#include "VulkanGpuImage.hpp"

#include <Usagi/Engine/Extension/Vulkan/VulkanEnumTranslation.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/GpuImageViewCreateInfo.hpp>

#include "VulkanGpuImageView.hpp"

using namespace usagi::vulkan;

void usagi::VulkanGpuImage::createBaseView()
{
    vk::ImageViewCreateInfo info;
    info.setImage(image());
    info.setViewType(vk::ImageViewType::e2D);
    info.setFormat(mFormat);
    info.setComponents(vk::ComponentMapping { });
    vk::ImageSubresourceRange subresource_range;
    subresource_range.setAspectMask(vk::ImageAspectFlagBits::eColor);
    subresource_range.setBaseArrayLayer(0);
    subresource_range.setLayerCount(1);
    subresource_range.setBaseMipLevel(0);
    subresource_range.setLevelCount(1);
    info.setSubresourceRange(subresource_range);

    mBaseView = std::make_shared<VulkanGpuImageView>(
        this, mDevice.createImageViewUnique(info));
}

usagi::VulkanGpuImage::VulkanGpuImage(
    vk::Device vk_device,
    vk::Format format)
    : mDevice(vk_device)
    , mFormat(format)
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
    vk_info.setFormat(mFormat);
    vk_info.components.r = translate(info.components.r);
    vk_info.components.g = translate(info.components.g);
    vk_info.components.b = translate(info.components.b);
    vk_info.components.a = translate(info.components.a);
    vk::ImageSubresourceRange subresource_range;
    subresource_range.setAspectMask(vk::ImageAspectFlagBits::eColor);
    subresource_range.setBaseArrayLayer(0);
    subresource_range.setLayerCount(1);
    subresource_range.setBaseMipLevel(0);
    subresource_range.setLevelCount(1);
    vk_info.setSubresourceRange(subresource_range);

    return std::make_shared<VulkanGpuImageView>(
        this, mDevice.createImageViewUnique(vk_info));
}
