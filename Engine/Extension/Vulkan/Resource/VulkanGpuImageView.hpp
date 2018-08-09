#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/Resource/GpuImageView.hpp>

namespace usagi
{
class VulkanGpuImageView : public GpuImageView
{
    vk::UniqueImageView mImageView;
    vk::Format mFormat;

public:
    VulkanGpuImageView(vk::UniqueImageView vk_image_view, vk::Format format);

    vk::ImageView view() const
    {
        return mImageView.get();
    }

    vk::Format format() const
    {
        return mFormat;
    }
};
}
