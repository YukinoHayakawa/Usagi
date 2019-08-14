#include "VulkanGpuImageView.hpp"

#include <Usagi/Core/Exception.hpp>

#include "VulkanGpuImage.hpp"

usagi::VulkanGpuImageView::VulkanGpuImageView(
    VulkanGpuImage *image,
    vk::UniqueImageView vk_image_view)
    : mImage(image)
    , mImageView(std::move(vk_image_view))
{
}

void usagi::VulkanGpuImageView::fillShaderResourceInfo(
    vk::WriteDescriptorSet &write,
    VulkanResourceInfo &info)
{
    auto &image_info = std::get<vk::DescriptorImageInfo>(info);
    switch(write.descriptorType)
    {
        case vk::DescriptorType::eSampledImage:
            image_info.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
            break;
        default:
            USAGI_THROW(std::runtime_error("Unsupported image usage."));
    }
    image_info.setImageView(mImageView.get());
    write.setPImageInfo(&image_info);
}

void usagi::VulkanGpuImageView::appendAdditionalResources(
    std::vector<std::shared_ptr<usagi::VulkanBatchResource>> &resources)
{
    resources.push_back(mImage->shared_from_this());
}
