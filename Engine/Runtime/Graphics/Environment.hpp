#pragma once

#include <memory>

namespace yuki::graphics
{
struct Environment
{
    std::unique_ptr<class Window> window;
    std::unique_ptr<class Device> graphics_device;
    std::unique_ptr<class SwapChain> swap_chain;
    std::unique_ptr<class ResourceManager> resource_manager;
    std::unique_ptr<class FrameController> frame_controller;

    ~Environment();
};
}
