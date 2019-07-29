#pragma once

#include <memory>

#include <Usagi/Math/Matrix.hpp>

namespace usagi
{
enum class GpuBufferFormat;
class Runtime;
class Swapchain;
class Window;

class RenderWindow
{
public:
    std::shared_ptr<Window> window;
    std::shared_ptr<Swapchain> swapchain;

    void create(Runtime *runtime,
        std::string_view window_title,
        const Vector2i &window_position,
        const Vector2u32 &window_size,
        GpuBufferFormat swapchain_format);

    void createCentered(Runtime *runtime,
        std::string_view window_title,
        const Vector2u32 &window_size,
        GpuBufferFormat swapchain_format);
};
}
