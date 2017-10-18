#pragma once

#include <memory>

namespace yuki
{

struct GraphicsEnvironment
{
    std::unique_ptr<class Window> window;
    std::unique_ptr<class GraphicsDevice2> graphics_device;
    std::unique_ptr<class SwapChain> swap_chain;
    std::unique_ptr<class GraphicsResourceAllocator> resource_allocator;
    std::unique_ptr<class FrameController> frame_control;
};

}
