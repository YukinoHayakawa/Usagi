#include "RenderWindow.hpp"

#include <Usagi/Runtime/Graphics/GpuDevice.hpp>
#include <Usagi/Runtime/Graphics/Swapchain.hpp>
#include <Usagi/Runtime/Runtime.hpp>
#include <Usagi/Runtime/Window/Window.hpp>
#include <Usagi/Runtime/Window/WindowManager.hpp>

void usagi::RenderWindow::create(
    Runtime *runtime,
    const std::string &window_title,
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
