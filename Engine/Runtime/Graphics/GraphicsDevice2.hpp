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
    virtual std::shared_ptr<class SwapChain> createSwapChain(std::shared_ptr<class Window> window) = 0;
    virtual std::shared_ptr<class GraphicsPipeline> createGraphicsPipeline() = 0;
    virtual std::shared_ptr<class GraphicsCommandPool> createGraphicsCommandPool() = 0;
    virtual std::shared_ptr<class VertexBuffer> createVertexBuffer(size_t size) = 0;

    virtual void submitGraphicsCommandList(
        class GraphicsCommandList *command_list,
        const std::vector<const GraphicsSemaphore *> &wait_semaphores,
        const std::vector<const GraphicsSemaphore *> &signal_semaphores
    ) = 0;

    virtual void waitIdle() = 0;
};

}
