#include "VulkanGpuImage.hpp"

#include "VulkanGpuImageView.hpp"

void usagi::VulkanGpuImage::createBaseView(
    vk::Device device,
    vk::Image image,
    vk::Format format)
{
    vk::ImageViewCreateInfo info;
    info.setImage(image);
    info.setViewType(vk::ImageViewType::e2D);
    info.setFormat(format);
    info.setComponents(vk::ComponentMapping { });
    vk::ImageSubresourceRange subresource_range;
    subresource_range.setAspectMask(vk::ImageAspectFlagBits::eColor);
    subresource_range.setBaseArrayLayer(0);
    subresource_range.setLayerCount(1);
    subresource_range.setBaseMipLevel(0);
    subresource_range.setLevelCount(1);
    info.setSubresourceRange(subresource_range);

    mBaseView = std::make_shared<VulkanGpuImageView>(
        this, device.createImageViewUnique(info), format);
}

std::shared_ptr<usagi::GpuImageView> usagi::VulkanGpuImage::baseView()
{
    return mBaseView;
}
