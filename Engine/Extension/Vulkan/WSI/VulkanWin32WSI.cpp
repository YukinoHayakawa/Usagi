#ifdef _WIN32

#include <Usagi/Engine/Extension/Win32/Win32.hpp>
#define VK_USE_PLATFORM_WIN32_KHR
#include "../VulkanGpuDevice.hpp"

#include <Usagi/Engine/Core/Logging.hpp>
#include <Usagi/Engine/Extension/Win32/Win32Window.hpp>
#include <Usagi/Engine/Extension/Vulkan/VulkanSwapChain.hpp>

std::shared_ptr<usagi::SwapChain> usagi::VulkanGpuDevice::createSwapChain(
    Window *window)
{
    auto &win32_window = dynamic_cast<Win32Window&>(*window);

    LOG(info, "Creating Win32Surface for window \"{}\"", win32_window.title());

    vk::Win32SurfaceCreateInfoKHR surface_create_info;
    surface_create_info.setHinstance(reinterpret_cast<HINSTANCE>(
        GetWindowLongPtrW(win32_window.handle(), GWLP_HINSTANCE)));
    surface_create_info.setHwnd(win32_window.handle());

    return std::make_shared<VulkanSwapChain>(this,
        mInstance->createWin32SurfaceKHRUnique(surface_create_info)
    );
}

#endif
