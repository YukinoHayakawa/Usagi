#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/GraphicsImage.hpp>

namespace yuki
{

class VulkanImage : public GraphicsImage
{
    vk::UniqueImageView mFullView;

public:
    VulkanImage(vk::UniqueImageView image_view);

    virtual vk::Image _getImage() const = 0;
    vk::ImageView _getFullView() const;

    static vk::ImageLayout translateImageLayout(enum class GraphicsImageLayout layout);
};

class VulkanSwapChainImage : public VulkanImage
{
    vk::Image mImage;

public:
    VulkanSwapChainImage(vk::Image image, vk::UniqueImageView image_view);

    vk::Image _getImage() const override;
};
}
