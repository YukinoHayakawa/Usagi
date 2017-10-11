#include <Usagi/Engine/Runtime/Exception.hpp>
#include <Usagi/Engine/Runtime/Graphics/GraphicsImageLayout.hpp>

#include "VulkanImage.hpp"

yuki::VulkanImage::VulkanImage(vk::UniqueImageView image_view)
    : mFullView { std::move(image_view) }
{
}

vk::ImageView yuki::VulkanImage::_getFullView() const
{
    return mFullView.get();
}

vk::ImageLayout yuki::VulkanImage::translateImageLayout(GraphicsImageLayout layout)
{
    switch(layout)
    {
        case GraphicsImageLayout::UNDEFINED: return vk::ImageLayout::eUndefined;
        case GraphicsImageLayout::GENERAL: return vk::ImageLayout::eGeneral;
        case GraphicsImageLayout::PRESENT_SRC: return vk::ImageLayout::ePresentSrcKHR;
        case GraphicsImageLayout::TRANSFER_DST: return vk::ImageLayout::eTransferDstOptimal;
        case GraphicsImageLayout::TRANSFER_SRC: return vk::ImageLayout::eTransferSrcOptimal;
        case GraphicsImageLayout::COLOR_ATTACHMENT: return vk::ImageLayout::eColorAttachmentOptimal;
        default: throw GraphicsAPIEnumTranslationException() << EnumInfo("vk::ImageLayout");
    }
}

yuki::VulkanSwapChainImage::VulkanSwapChainImage(vk::Image image, vk::UniqueImageView image_view)
    : VulkanImage{ std::move(image_view) }
    , mImage{ std::move(image) }
{
}

vk::Image yuki::VulkanSwapChainImage::_getImage() const
{
    return mImage;
}
