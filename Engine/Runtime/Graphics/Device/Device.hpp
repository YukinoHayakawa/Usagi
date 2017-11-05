#pragma once

#include <memory>
#include <vector>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace yuki
{

class Window;

}

namespace yuki::graphics
{

class Waitable;

/**
 * \brief Allocate GPU resources and simplify the interaction with modern graphics APIs.
 * Manage work submission to GPU.
 */
class Device : Noncopyable
{
public:
    virtual ~Device() = default;

    // todo: remove param
    virtual std::unique_ptr<class SwapChain> createSwapChain(Window *window) = 0;
    virtual std::unique_ptr<class Pipeline> createPipeline(const struct PipelineCreateInfo &info) = 0;
    virtual std::unique_ptr<class FrameController> createFrameController(size_t num_frames) = 0;
    virtual std::unique_ptr<class CommandPool> createGraphicsCommandPool() = 0;
    virtual std::unique_ptr<class Sampler> createSampler(const struct SamplerCreateInfo &info) = 0;

    /**
     * \brief Submit a graphics command list to the GPU and synchronize its execution.
     * \param command_list 
     * \param device_wait_objects 
     * \param device_signal_objects 
     * \param host_sync_fence 
     */
    virtual void submitGraphicsCommandList(
        class CommandList *command_list,
        const std::vector<Waitable *> &device_wait_objects,
        const std::vector<Waitable *> &device_signal_objects,
        Waitable *host_sync_fence
    ) = 0;

    virtual void waitIdle() = 0;
};

}
