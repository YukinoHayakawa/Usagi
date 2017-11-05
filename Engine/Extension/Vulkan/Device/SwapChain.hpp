#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

#include "Semaphore.hpp"

#include <Usagi/Engine/Runtime/Graphics/Device/SwapChain.hpp>

namespace yuki::vulkan
{

/**
 * \brief Present render result to window system.
 */
class SwapChain : public graphics::SwapChain
{
    class Device *mVulkanGD;

    vk::UniqueSurfaceKHR mSurface;
    vk::Format mSurfaceFormat = vk::Format::eUndefined;
    vk::UniqueSwapchainKHR mSwapChain;
    std::vector<std::unique_ptr<class SwapChainImage>> mSwapChainImages;
    uint32_t mCurrentImageIndex = 0;
    Semaphore mImageAvailableSemaphore;

    uint32_t mPresentationQueueFamilyIndex = 0;

    static vk::SurfaceFormatKHR _selectSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &surface_formats);
    static vk::Extent2D _selectSurfaceExtent(const vk::SurfaceCapabilitiesKHR &surface_capabilities);
    static vk::PresentModeKHR _selectPresentMode(const std::vector<vk::PresentModeKHR> &present_modes);
    uint32_t _selectPresentationQueueFamily() const;

    /**
     * \brief Initialize the swap chain, or create a new swap chain after the window changes its size.
     */
    void _createSwapChain();

public:
    // todo create with window
    SwapChain(Device *device, HINSTANCE hInstance, HWND hWnd);

    void acquireNextImage() override;
    graphics::Image * getCurrentImage() override;
    void present(const std::vector<graphics::Waitable *> &wait_semaphores) override;

    graphics::Waitable * getImageAvailableSemaphore() override;

    uint32_t getNativeImageFormat() override;
};

}
