#ifdef _WIN32

#include <Usagi/Engine/Extension/Win32/Win32.hpp>
#define VK_USE_PLATFORM_WIN32_KHR
#include "../VulkanGpuDevice.hpp"

#include <Usagi/Engine/Core/Logging.hpp>
#include <Usagi/Engine/Utility/TypeCast.hpp>
#include <Usagi/Engine/Extension/Win32/Window/Win32Window.hpp>

#include "VulkanSwapchain.hpp"

std::shared_ptr<usagi::Swapchain> usagi::VulkanGpuDevice::createSwapchain(
    Window *window)
{
    auto &win32_window = dynamic_cast_ref<Win32Window>(window);

    LOG(info, "Creating Win32Surface for window: {}", win32_window.title());

    vk::Win32SurfaceCreateInfoKHR surface_create_info;
    surface_create_info.setHinstance(reinterpret_cast<HINSTANCE>(
        GetWindowLongPtrW(win32_window.handle(), GWLP_HINSTANCE)));
    surface_create_info.setHwnd(win32_window.handle());

    return std::make_shared<VulkanSwapchain>(this, window,
        mInstance->createWin32SurfaceKHRUnique(surface_create_info)
    );
}

void usagi::VulkanGpuDevice::checkQueuePresentationCapacity(
    const uint32_t queue_family_index) const
{
    assert(mPhysicalDevice.getWin32PresentationSupportKHR(queue_family_index));
}

#endif
