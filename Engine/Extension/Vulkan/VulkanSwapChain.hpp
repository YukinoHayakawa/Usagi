#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Graphics/SwapChain.hpp>

namespace usagi
{
class VulkanGpuDevice;

class VulkanSwapChain : public SwapChain
{
    VulkanGpuDevice *mDevice;

    std::uint32_t mPresentationQueueFamilyIndex = 0;

    vk::UniqueSurfaceKHR mSurface;
    vk::Format mSurfaceFormat = vk::Format::eUndefined;

    vk::UniqueSwapchainKHR mSwapChain;
    std::vector<vk::UniqueImageView> mSwapChainImageViews;
    uint32_t mCurrentImageIndex = 0;
    
    vk::Semaphore mImageAvailableSemaphore;

    static vk::SurfaceFormatKHR selectSurfaceFormat(
        const std::vector<vk::SurfaceFormatKHR> &surface_formats);
    static vk::Extent2D selectSurfaceExtent(
        const vk::SurfaceCapabilitiesKHR &surface_capabilities);
    static vk::PresentModeKHR selectPresentMode(
        const std::vector<vk::PresentModeKHR> &present_modes);
    std::uint32_t selectPresentationQueueFamily() const;

    void createSwapChain();

public:
    VulkanSwapChain(
        VulkanGpuDevice *device,
        vk::UniqueSurfaceKHR vk_surface_khr);

    void acquireNextImage() override;
    GpuImage * getCurrentImage() override;
};
}
