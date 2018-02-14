#include <Usagi/Engine/Utility/Math.hpp>
#include <Usagi/Engine/Runtime/Exception.hpp>

#include "SwapChain.hpp"
#include "Device.hpp"
#include "../Resource/Image.hpp"

namespace yuki::extension::vulkan
{
vk::SurfaceFormatKHR SwapChain::_selectSurfaceFormat(
    const std::vector<vk::SurfaceFormatKHR> &surface_formats)
{
    // If the list contains only one entry with undefined format
    // it means that there are no preferred surface formats and any can be chosen
    if(surface_formats.size() == 1 && surface_formats[0].format == vk::Format::eUndefined)
    {
        return { vk::Format::eR8G8B8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
    }

    // Check if list contains most widely used R8 G8 B8 A8 format
    // with nonlinear color space
    for(auto &&surface_format : surface_formats)
    {
        if(surface_format.format == vk::Format::eR8G8B8A8Unorm)
        {
            return surface_format;
        }
    }

    // Return the first format from the list
    return surface_formats[0];
}

vk::Extent2D SwapChain::_selectSurfaceExtent(
    const vk::SurfaceCapabilitiesKHR &surface_capabilities)
{
    // Special value of surface extent is width == height == -1
    // If this is so we define the size by ourselves but it must fit within defined confines
    if(surface_capabilities.currentExtent.width == -1)
    {
        vk::Extent2D swap_chain_extent = { 1280, 720 };
        swap_chain_extent.width = clamp(swap_chain_extent.width, {
            surface_capabilities.minImageExtent.width,
            surface_capabilities.maxImageExtent.width
        });
        swap_chain_extent.height = clamp(swap_chain_extent.height, {
            surface_capabilities.minImageExtent.height,
            surface_capabilities.maxImageExtent.height
        });
        return swap_chain_extent;
    }
    // Most of the cases we define size of the swap_chain images equal to current window's size
    return surface_capabilities.currentExtent;
}

vk::PresentModeKHR SwapChain::_selectPresentMode(
    const std::vector<vk::PresentModeKHR> &present_modes)
{
    return vk::PresentModeKHR::eImmediate;

    // prefer mailbox mode to achieve triple buffering
    if(find(present_modes.begin(), present_modes.end(), vk::PresentModeKHR::eMailbox) !=
        present_modes.end())
        return vk::PresentModeKHR::eMailbox;
    return vk::PresentModeKHR::eFifo;
}

uint32_t SwapChain::_selectPresentationQueueFamily() const
{
    const auto queue_families = mVulkanGD->physicalDevice().getQueueFamilyProperties();
    for(auto iter = queue_families.begin(); iter != queue_families.end(); ++iter)
    {
        const uint32_t queue_index = iter - queue_families.begin();
        if(mVulkanGD->physicalDevice().getSurfaceSupportKHR(queue_index, mSurface.get()))
            return queue_index;
    }
    throw GraphicsAPIUnsupportedFeatureException() << FeatureDescriptionInfo(
        "no queue family supporting WSI was found");
}

// todo recreate after resizing the window
void SwapChain::_createSwapChain()
{
    mPresentationQueueFamilyIndex = _selectPresentationQueueFamily();

    const auto surface_capabilities = mVulkanGD->physicalDevice().
        getSurfaceCapabilitiesKHR(mSurface.get());
    const auto surface_formats = mVulkanGD->physicalDevice().getSurfaceFormatsKHR(
        mSurface.get());
    const auto surface_present_modes = mVulkanGD->physicalDevice().
        getSurfacePresentModesKHR(mSurface.get());

    vk::SwapchainCreateInfoKHR swapchain_create_info_khr;

    swapchain_create_info_khr.setSurface(mSurface.get());
    const auto format = _selectSurfaceFormat(surface_formats);
    mSurfaceFormat = format.format;
    swapchain_create_info_khr.setImageFormat(format.format);
    swapchain_create_info_khr.setImageColorSpace(format.colorSpace);
    swapchain_create_info_khr.setImageExtent(_selectSurfaceExtent(surface_capabilities));

    // set up triple buffering
    const uint32_t image_count = 3;
    if(!withinOpenInterval(image_count, {
        surface_capabilities.minImageCount,
        surface_capabilities.maxImageCount
    }))
        throw GraphicsAPIUnsupportedFeatureException() << FeatureDescriptionInfo(
            "the device does not support triple buffering");
    swapchain_create_info_khr.setMinImageCount(image_count);
    swapchain_create_info_khr.setPresentMode(_selectPresentMode(surface_present_modes));

    swapchain_create_info_khr.setImageArrayLayers(1);
    swapchain_create_info_khr.setImageSharingMode(vk::SharingMode::eExclusive);
    swapchain_create_info_khr.setPreTransform(surface_capabilities.currentTransform);

    swapchain_create_info_khr.setImageUsage(
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst);

    swapchain_create_info_khr.setOldSwapchain(mSwapChain.get());

    mSwapChain = mVulkanGD->device().createSwapchainKHRUnique(swapchain_create_info_khr);
}

SwapChain::SwapChain(Device *device, HINSTANCE hInstance, HWND hWnd)
    : mVulkanGD(std::move(device))
{
    vk::SemaphoreCreateInfo semaphore_create_info;
    mImageAvailableSemaphore = {
        mVulkanGD->device().createSemaphoreUnique(semaphore_create_info),
        vk::PipelineStageFlagBits::eColorAttachmentOutput
    };

    vk::Win32SurfaceCreateInfoKHR surface_create_info;
    surface_create_info.setHinstance(hInstance);
    surface_create_info.setHwnd(hWnd);
    mSurface = mVulkanGD->_getInstance().createWin32SurfaceKHRUnique(surface_create_info);

    _createSwapChain();

    auto images = mVulkanGD->device().getSwapchainImagesKHR(mSwapChain.get());
    for(auto &&image : images)
    {
        vk::ImageViewCreateInfo image_view_create_info;
        image_view_create_info.setImage(image);
        image_view_create_info.setViewType(vk::ImageViewType::e2D);
        image_view_create_info.setFormat(mSurfaceFormat);
        vk::ImageSubresourceRange image_subresource_range;
        image_subresource_range.setAspectMask(vk::ImageAspectFlagBits::eColor);
        image_subresource_range.setBaseMipLevel(0);
        image_subresource_range.setLevelCount(1);
        image_subresource_range.setBaseArrayLayer(0);
        image_subresource_range.setLayerCount(1);
        image_view_create_info.setSubresourceRange(image_subresource_range);
        auto full_image_view = mVulkanGD->device().createImageViewUnique(
            image_view_create_info);
        mSwapChainImages.push_back(
            std::make_unique<SwapChainImage>(image, std::move(full_image_view)));
    }
}

void SwapChain::acquireNextImage()
{
    const auto result = mVulkanGD->device().acquireNextImageKHR(
        mSwapChain.get(),
        UINT64_MAX,
        mImageAvailableSemaphore._getSemaphore(), { }
    );
    switch(result.result)
    {
        case vk::Result::eSuccess: break;
        case vk::Result::eNotReady:
        case vk::Result::eTimeout:
        case vk::Result::eSuboptimalKHR:
        default: throw GraphicsSwapChainNotAvailableException();
    }
    mCurrentImageIndex = result.value;
}

graphics::Image * SwapChain::getCurrentImage()
{
    return mSwapChainImages[mCurrentImageIndex].get();
}

void SwapChain::present(const std::vector<graphics::Waitable *> &wait_semaphores)
{
    std::vector<vk::Semaphore> vulkan_semaphores = Semaphore::convertToVulkanHandles(
        wait_semaphores, nullptr);

    vk::PresentInfoKHR present_info;
    present_info.setWaitSemaphoreCount(vulkan_semaphores.size());
    present_info.setPWaitSemaphores(vulkan_semaphores.data());
    // todo multi window presentation
    present_info.setSwapchainCount(1);
    present_info.setPSwapchains(&*mSwapChain);
    present_info.setPImageIndices(&mCurrentImageIndex);
    // todo present to present queue
    mVulkanGD->_getGraphicsQueue().presentKHR(present_info);
}

graphics::Waitable * SwapChain::getImageAvailableSemaphore()
{
    return &mImageAvailableSemaphore;
}

uint32_t SwapChain::getNativeImageFormat()
{
    return static_cast<uint32_t>(mSurfaceFormat);
}
}
