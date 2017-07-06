#pragma once

#include <memory>

#include "Window/Window.hpp"
#include "GraphicsInterface/GraphicsInterface.hpp"

namespace yuki
{

/**
 * \brief A wrapper class providing the same functionality of Window and GraphicsInterface.
 * It is usually created by a DevicePlatform implementation, but can also be setup manually.
 */
class RenderWindow final : public Window, public GraphicsInterface
{
    std::shared_ptr<Window> mWindow;
    std::shared_ptr<GraphicsInterface> mGI;

public:
    RenderWindow(std::shared_ptr<Window> window, std::shared_ptr<GraphicsInterface> graphics_interface);

    void show() override;
    void hide() override;
    void swapBuffer() override;
    void setCurrent() override;
    void processEvents() override;
};

}
