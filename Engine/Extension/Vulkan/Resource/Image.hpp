#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/Resource/Image.hpp>

namespace yuki::graphics
{

enum class ImageLayout;

}
namespace yuki::vulkan
{

class Image : public graphics::Image
{
    vk::UniqueImageView mFullView;

public:
    Image(vk::UniqueImageView image_view);

    virtual vk::Image _getImage() const = 0;
    vk::ImageView _getFullView() const;

    static vk::ImageLayout translateImageLayout(graphics::ImageLayout layout);
};

class SwapChainImage : public Image
{
    vk::Image mImage;

public:
    SwapChainImage(vk::Image image, vk::UniqueImageView image_view);

    vk::Image _getImage() const override;
};

}
