#pragma once

#include <memory>

#include <Usagi/Core/Math.hpp>
#include <Usagi/Runtime/Graphics/Enum/GpuBufferFormat.hpp>

namespace usagi
{
class Runtime;
class Swapchain;
class Window;

class RenderWindow
{
    std::shared_ptr<Window> mWindow;
    std::shared_ptr<Swapchain> mSwapchain;

public:
    void create(Runtime *runtime,
        const std::string &window_title,
        const Vector2i &window_position,
        const Vector2u32 &window_size,
        GpuBufferFormat swapchain_format = GpuBufferFormat::R8G8B8A8_UNORM);

    Window * window() const { return mWindow.get(); }
    Swapchain * swapchain() const { return mSwapchain.get(); }
};
}
