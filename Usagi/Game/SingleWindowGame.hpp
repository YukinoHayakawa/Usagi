#pragma once

#include <Usagi/Runtime/Window/WindowEventListener.hpp>
#include <Usagi/Runtime/Graphics/Enum/GpuBufferFormat.hpp>

#include "Game.hpp"

namespace usagi
{
class Swapchain;
class Window;

/**
 * \brief Provides the following functionalities:
 *
 * Initialize the window, input, GPU systems of runtime.
 * A single window with a swapchain.
 * Auto-resize swapchain upon window resize.
 */
class SingleWindowGame
    : public Game
    , public WindowEventListener
{
protected:
    std::shared_ptr<Window> mWindow;
    std::shared_ptr<Swapchain> mSwapchain;

public:
    SingleWindowGame(
        Runtime *runtime,
        const std::string &window_title,
        const Vector2i &window_position,
        const Vector2u32 &window_size,
        GpuBufferFormat swapchain_format = GpuBufferFormat::R8G8B8A8_UNORM);

    void onWindowResizeEnd(const WindowSizeEvent &e) override;
};
}
