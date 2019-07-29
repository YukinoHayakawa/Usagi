#include "RenderWindow.hpp"

#include <Usagi/Runtime/Graphics/GpuDevice.hpp>
#include <Usagi/Runtime/Graphics/Swapchain.hpp>
#include <Usagi/Runtime/Runtime.hpp>
#include <Usagi/Runtime/Window/Window.hpp>
#include <Usagi/Runtime/Window/WindowManager.hpp>

void usagi::RenderWindow::create(
    Runtime *runtime,
    std::string_view window_title,
    const Vector2i &window_position,
    const Vector2u32 &window_size,
    const GpuBufferFormat swapchain_format)
{
    runtime->initWindow();
    runtime->initGpu();

    window = runtime->windowManager()->createWindow(
        window_title, window_position, window_size);

    swapchain = runtime->gpu()->createSwapchain(window.get());
    swapchain->create(window->size(), swapchain_format);
}

void usagi::RenderWindow::createCentered(
    Runtime *runtime,
    std::string_view window_title,
    const Vector2u32 &window_size,
    GpuBufferFormat swapchain_format)
{
    const auto display_size =
        runtime->windowManager()->currentDisplayResolution();
    const Vector2i centered_pos =
        (display_size.cast<int>() - window_size.cast<int>()) / 2;
    create(runtime, window_title, centered_pos, window_size, swapchain_format);
}
