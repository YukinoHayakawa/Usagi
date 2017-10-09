#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/SwapChain.hpp>

namespace yuki
{

/**
 * \brief Present render result to window system.
 */
class VulkanSwapChain : public SwapChain
{
    std::shared_ptr<class VulkanGraphicsDevice> mVulkanGD;

    vk::UniqueSemaphore mImageAvailableSemaphore, mRenderingFinishedSemaphore;
    vk::UniqueSurfaceKHR mSurface;
    vk::Format mSurfaceFormat;
    vk::UniqueSwapchainKHR mSwapChain;
    std::vector<vk::Image> mSwapChainImages;
    vk::UniqueCommandPool mPresentCommandPool;
    std::vector<vk::UniqueCommandBuffer> mPresentCommandBuffers;

    uint32_t mPresentationQueueFamilyIndex = 0;

    static vk::SurfaceFormatKHR _selectSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &surface_formats);
    static vk::Extent2D _selectSurfaceExtent(const vk::SurfaceCapabilitiesKHR &surface_capabilities);
    static vk::PresentModeKHR _selectPresentMode(const std::vector<vk::PresentModeKHR> &present_modes);
    uint32_t _selectPresentationQueueFamily() const;

    /**
     * \brief Initialize the swap chain, or create a new swap chain after the window changes its size.
     */
    void _createSwapChain();
    void _createCommandBuffers();
    void _recordCommands();

public:
    VulkanSwapChain(std::shared_ptr<VulkanGraphicsDevice> device, HINSTANCE hInstance, HWND hWnd);
    ~VulkanSwapChain() override;

    void present() override;

    uint64_t getSurfaceFormat() override;
};

}
