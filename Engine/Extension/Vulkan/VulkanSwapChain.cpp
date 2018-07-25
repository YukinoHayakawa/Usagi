#include "VulkanSwapChain.hpp"

#include <Usagi/Engine/Utility/Math.hpp>

#include "VulkanGpuDevice.hpp"
#include <Usagi/Engine/Core/Logging.hpp>

usagi::VulkanSwapChain::VulkanSwapChain(
    VulkanGpuDevice *device,
    vk::UniqueSurfaceKHR vk_surface_khr)
    : mDevice { device }
    , mSurface { std::move(vk_surface_khr) }
{
    createSwapChain();
}

void usagi::VulkanSwapChain::acquireNextImage()
{
}

usagi::GpuImage * usagi::VulkanSwapChain::getCurrentImage()
{
    throw 0;
}

// todo: better selection algorithm
vk::SurfaceFormatKHR usagi::VulkanSwapChain::selectSurfaceFormat(
    const std::vector<vk::SurfaceFormatKHR> &surface_formats)
{
    // If the list contains only one entry with undefined format
    // it means that there are no preferred surface formats and any can be chosen
    if(surface_formats.size() == 1 &&
        surface_formats[0].format == vk::Format::eUndefined)
        return {
            vk::Format::eR8G8B8A8Unorm,
            vk::ColorSpaceKHR::eSrgbNonlinear
        };

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

vk::Extent2D usagi::VulkanSwapChain::selectSurfaceExtent(
    const vk::SurfaceCapabilitiesKHR &surface_capabilities)
{
    // Special value of surface extent is width == height == -1
    // If this is so we define the size by ourselves but it must fit within
    // defined confines
    if(surface_capabilities.currentExtent.width == -1)
    {
        vk::Extent2D swap_chain_extent = { 1280, 720 };
        swap_chain_extent.width = std::clamp(swap_chain_extent.width,
            surface_capabilities.minImageExtent.width,
            surface_capabilities.maxImageExtent.width
        );
        swap_chain_extent.height = std::clamp(swap_chain_extent.height,
            surface_capabilities.minImageExtent.height,
            surface_capabilities.maxImageExtent.height
        );
        return swap_chain_extent;
    }
    // Most of the cases we define size of the swap_chain images equal to
    // current window's size
    return surface_capabilities.currentExtent;
}

vk::PresentModeKHR usagi::VulkanSwapChain::selectPresentMode(
    const std::vector<vk::PresentModeKHR> &present_modes)
{
    // prefer mailbox mode to achieve triple buffering
    if(std::find(present_modes.begin(), present_modes.end(),
            vk::PresentModeKHR::eMailbox) != present_modes.end())
        return vk::PresentModeKHR::eMailbox;
    return vk::PresentModeKHR::eFifo;
}

std::uint32_t usagi::VulkanSwapChain::selectPresentationQueueFamily() const
{
    const auto queue_families =
        mDevice->physicalDevice().getQueueFamilyProperties();
    for(auto i = queue_families.begin(); i != queue_families.end(); ++i)
    {
        const auto queue_index =
            static_cast<uint32_t>(i - queue_families.begin());
        if(mDevice->physicalDevice().getSurfaceSupportKHR(
            queue_index, mSurface.get()))
            return queue_index;
    }
    throw std::runtime_error("No queue family supporting WSI was found.");
}

void usagi::VulkanSwapChain::createSwapChain()
{
    LOG(info, "Creating swapchain");

    mPresentationQueueFamilyIndex = selectPresentationQueueFamily();
    LOG(info, "Using queue family {} for presentation",
        mPresentationQueueFamilyIndex);

    const auto surface_capabilities =
        mDevice->physicalDevice().getSurfaceCapabilitiesKHR(mSurface.get());
    const auto surface_formats =
        mDevice->physicalDevice().getSurfaceFormatsKHR(mSurface.get());
    const auto surface_present_modes =
        mDevice->physicalDevice().getSurfacePresentModesKHR(mSurface.get());

    vk::SwapchainCreateInfoKHR create_info;

    create_info.setSurface(mSurface.get());
    const auto format = selectSurfaceFormat(surface_formats);
    mSurfaceFormat = format.format;
    LOG(info, "Surface format: {}", to_string(mSurfaceFormat));
    create_info.setImageFormat(format.format);
    create_info.setImageColorSpace(format.colorSpace);
    create_info.setImageExtent(selectSurfaceExtent(surface_capabilities));
    LOG(info, "Surface extent: {}x{}",
        create_info.imageExtent.width, create_info.imageExtent.height);

    // Ensures non-blocking vkAcquireNextImageKHR() in mailbox mode.
    // See 3.6.12 of http://vulkan-spec-chunked.ahcox.com/apes03.html
    create_info.setMinImageCount(surface_capabilities.minImageCount + 1);
    create_info.setPresentMode(selectPresentMode(surface_present_modes));

    create_info.setImageArrayLayers(1);
    create_info.setImageSharingMode(vk::SharingMode::eExclusive);
    create_info.setPreTransform(surface_capabilities.currentTransform);

    create_info.setImageUsage(
        vk::ImageUsageFlagBits::eColorAttachment |
        vk::ImageUsageFlagBits::eTransferDst
    );

    create_info.setOldSwapchain(mSwapChain.get());

    mSwapChain = mDevice->device().createSwapchainKHRUnique(
        create_info);
}
