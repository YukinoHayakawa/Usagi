#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/SwapChain.hpp>
#include <Usagi/Engine/Utility/Exception.hpp>

YUKI_DECL_RUNTIME_EXCEPTION(VulkanRuntime);

namespace yuki
{

/**
 * \brief Present render result to window system.
 */
class VulkanSwapChain : public SwapChain
{
    std::shared_ptr<class VulkanGraphicsDevice> mDevice;

    vk::Semaphore mPresentationSemaphore;
    vk::SurfaceKHR mSurface;
    vk::SwapchainKHR mSwapChain;
    uint32_t mPresentationQueueFamilyIndex = 0;

    static vk::SurfaceFormatKHR _selectSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &surface_formats);
    static vk::Extent2D _selectSurfaceExtent(const vk::SurfaceCapabilitiesKHR &surface_capabilities);
    static vk::PresentModeKHR _selectPresentMode(const std::vector<vk::PresentModeKHR> &present_modes);
    uint32_t _selectPresentationQueueFamily() const;

public:
    VulkanSwapChain(std::shared_ptr<VulkanGraphicsDevice> device, HINSTANCE hInstance, HWND hWnd);
    ~VulkanSwapChain() override;
};

}
