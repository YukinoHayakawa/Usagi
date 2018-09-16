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
public:
    std::shared_ptr<Window> window;
    std::shared_ptr<Swapchain> swapchain;

    void create(Runtime *runtime,
        const std::string &window_title,
        const Vector2i &window_position,
        const Vector2u32 &window_size,
        GpuBufferFormat swapchain_format = GpuBufferFormat::R8G8B8A8_UNORM);
};
}
