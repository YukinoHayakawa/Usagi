#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/Resource/GpuImageView.hpp>

namespace usagi
{
class VulkanGpuImageView : public GpuImageView
{
    vk::UniqueImageView mImageView;

public:
    explicit VulkanGpuImageView(vk::UniqueImageView vk_image_view);

    vk::ImageView view() const
    {
        return mImageView.get();
    }
};
}
