#pragma once

#include <memory>

namespace yuki
{

class GraphicsSemaphore;

/**
 * \brief Interface to graphics APIs.
 */
class GraphicsDevice2
{
public:
    virtual ~GraphicsDevice2() = default;

    // todo: remove param
    virtual std::unique_ptr<class SwapChain> createSwapChain(class Window *window) = 0;
    virtual std::unique_ptr<class GraphicsPipeline> createGraphicsPipeline() = 0;
    virtual std::unique_ptr<class FrameController> createFrameController(size_t num_frames) = 0;
    virtual std::unique_ptr<class GraphicsCommandPool> createGraphicsCommandPool() = 0;

    virtual void submitGraphicsCommandList(
        class GraphicsCommandList *command_list,
        const std::vector<GraphicsSemaphore *> &wait_semaphores,
        const std::vector<GraphicsSemaphore *> &signal_semaphores
    ) = 0;

    virtual void waitIdle() = 0;
};

}
