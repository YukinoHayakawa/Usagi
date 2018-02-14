#include <Usagi/Engine/Runtime/Exception.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/ImageLayout.hpp>

#include "Image.hpp"

using namespace yuki::graphics;

yuki::extension::vulkan::Image::Image(vk::UniqueImageView image_view)
    : mFullView { std::move(image_view) }
{
}

vk::ImageView yuki::extension::vulkan::Image::_getFullView() const
{
    return mFullView.get();
}

vk::ImageLayout yuki::extension::vulkan::Image::translateImageLayout(ImageLayout layout)
{
    switch(layout)
    {
        case ImageLayout::UNDEFINED: return vk::ImageLayout::eUndefined;
        case ImageLayout::GENERAL: return vk::ImageLayout::eGeneral;
        case ImageLayout::PRESENT_SRC: return vk::ImageLayout::ePresentSrcKHR;
        case ImageLayout::TRANSFER_DST: return vk::ImageLayout::eTransferDstOptimal;
        case ImageLayout::TRANSFER_SRC: return vk::ImageLayout::eTransferSrcOptimal;
        case ImageLayout::COLOR_ATTACHMENT: return vk::ImageLayout::eColorAttachmentOptimal;
        default: throw GraphicsAPIEnumTranslationException() << EnumInfo("vk::ImageLayout");
    }
}

yuki::extension::vulkan::SwapChainImage::SwapChainImage(vk::Image image, vk::UniqueImageView image_view)
    : vulkan::Image { std::move(image_view) }
    , mImage { std::move(image) }
{
}

vk::Image yuki::extension::vulkan::SwapChainImage::_getImage() const
{
    return mImage;
}
