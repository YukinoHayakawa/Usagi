#include <Usagi/Engine/Utility/Math.hpp>

#include "VulkanSwapChain.hpp"
#include "VulkanGraphicsDevice.hpp"

vk::SurfaceFormatKHR yuki::VulkanSwapChain::_selectSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &surface_formats)
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

vk::Extent2D yuki::VulkanSwapChain::_selectSurfaceExtent(const vk::SurfaceCapabilitiesKHR &surface_capabilities)
{
    // Special value of surface extent is width == height == -1
    // If this is so we define the size by ourselves but it must fit within defined confines
    if(surface_capabilities.currentExtent.width == -1)
    {
        vk::Extent2D swap_chain_extent = { 1280, 720 };
        swap_chain_extent.width = clamp(swap_chain_extent.width, { surface_capabilities.minImageExtent.width, surface_capabilities.maxImageExtent.width });
        swap_chain_extent.height = clamp(swap_chain_extent.height, { surface_capabilities.minImageExtent.height, surface_capabilities.maxImageExtent.height });
        return swap_chain_extent;
    }
    // Most of the cases we define size of the swap_chain images equal to current window's size
    return surface_capabilities.currentExtent;
}

vk::PresentModeKHR yuki::VulkanSwapChain::_selectPresentMode(const std::vector<vk::PresentModeKHR> &present_modes)
{
    if(find(present_modes.begin(), present_modes.end(), vk::PresentModeKHR::eMailbox) != present_modes.end())
        return vk::PresentModeKHR::eMailbox;
    return vk::PresentModeKHR::eFifo;
}

uint32_t yuki::VulkanSwapChain::_selectPresentationQueueFamily() const
{
    const auto queue_families = mDevice->mPhysicalDevice.getQueueFamilyProperties();
    for(auto iter = queue_families.begin(); iter != queue_families.end(); ++iter)
    {
        const uint32_t queue_index = iter - queue_families.begin();
        if(mDevice->mPhysicalDevice.getSurfaceSupportKHR(queue_index, mSurface))
            return queue_index;
    }
    throw VulkanRuntimeException("no queue family supporting WSI was found");
}

yuki::VulkanSwapChain::VulkanSwapChain(std::shared_ptr<VulkanGraphicsDevice> device, HINSTANCE hInstance, HWND hWnd)
    : mDevice(std::move(device))
{
    vk::SemaphoreCreateInfo semaphore_create_info;
    mPresentationSemaphore = mDevice->mDevice.createSemaphore(semaphore_create_info);

    vk::Win32SurfaceCreateInfoKHR surface_create_info;
    surface_create_info.setHinstance(hInstance);
    surface_create_info.setHwnd(hWnd);
    mSurface = mDevice->mInstance.createWin32SurfaceKHR(surface_create_info);
    
    mPresentationQueueFamilyIndex = _selectPresentationQueueFamily();

    const auto surface_capabilities = mDevice->mPhysicalDevice.getSurfaceCapabilitiesKHR(mSurface);
    const auto surface_formats = mDevice->mPhysicalDevice.getSurfaceFormatsKHR(mSurface);
    const auto surface_present_modes = mDevice->mPhysicalDevice.getSurfacePresentModesKHR(mSurface);

    vk::SwapchainCreateInfoKHR swapchain_create_info_khr;
    
    swapchain_create_info_khr.setSurface(mSurface);
    const auto format = _selectSurfaceFormat(surface_formats);
    swapchain_create_info_khr.setImageFormat(format.format);
    swapchain_create_info_khr.setImageColorSpace(format.colorSpace);
    swapchain_create_info_khr.setImageExtent(_selectSurfaceExtent(surface_capabilities));
    
    // set up triple buffering
    const uint32_t image_count = 3;
    checkThrow<VulkanRuntimeException>(
        withinOpenInterval(image_count, { surface_capabilities.minImageCount, surface_capabilities.maxImageCount }),
        "the device does not support triple buffering"
    );
    swapchain_create_info_khr.setMinImageCount(image_count);
    swapchain_create_info_khr.setPresentMode(_selectPresentMode(surface_present_modes));

    swapchain_create_info_khr.setImageArrayLayers(1);
    swapchain_create_info_khr.setImageSharingMode(vk::SharingMode::eExclusive);
    swapchain_create_info_khr.setPreTransform(surface_capabilities.currentTransform);

    swapchain_create_info_khr.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

    mSwapChain = mDevice->mDevice.createSwapchainKHR(swapchain_create_info_khr);
}

yuki::VulkanSwapChain::~VulkanSwapChain()
{
    mDevice->mDevice.destroySwapchainKHR(mSwapChain);
    mDevice->mDevice.destroySemaphore(mPresentationSemaphore);
    mDevice->mInstance.destroySurfaceKHR(mSurface);
}
