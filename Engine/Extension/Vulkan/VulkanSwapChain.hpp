#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

#include "VulkanSemaphore.hpp"

#include <Usagi/Engine/Runtime/Graphics/SwapChain.hpp>

namespace yuki
{

/**
 * \brief Present render result to window system.
 */
class VulkanSwapChain : public SwapChain
{
    class VulkanGraphicsDevice *mVulkanGD;

    vk::UniqueSurfaceKHR mSurface;
    vk::Format mSurfaceFormat = vk::Format::eUndefined;
    vk::UniqueSwapchainKHR mSwapChain;
    std::vector<std::unique_ptr<class VulkanSwapChainImage>> mSwapChainImages;
    uint32_t mCurrentImageIndex = 0;
    VulkanSemaphore mImageAvailableSemaphore;

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
    VulkanSwapChain(VulkanGraphicsDevice *device, HINSTANCE hInstance, HWND hWnd);

    void acquireNextImage() override;
    GraphicsImage * getCurrentImage() override;
    void present(const std::vector<GraphicsSemaphore *> wait_semaphores) override;

    GraphicsSemaphore * getImageAvailableSemaphore() override;

    uint32_t getNativeImageFormat() override;
};

}
