#pragma once

#include <memory>
#include <vector>

#include <Usagi/Engine/Utility/Noncopyable.hpp>
#include <Usagi/Engine/Core/Math.hpp>

namespace usagi
{
class GpuImageView;
class Framebuffer;
struct RenderPassCreateInfo;
class GpuImage;
class Swapchain;
class Window;
class GpuCommandPool;
class GraphicsCommandList;
class GraphicsPipelineCompiler;
class GpuSemaphore;
class RenderPass;
enum class GraphicsPipelineStage;

class GpuDevice : Noncopyable
{
public:
    virtual ~GpuDevice() = default;

    virtual std::shared_ptr<Swapchain> createSwapchain(Window *window) = 0;
    virtual std::unique_ptr<GpuCommandPool> createCommandPool() = 0;
    virtual std::unique_ptr<GraphicsPipelineCompiler>
        createPipelineCompiler() = 0;
    virtual std::shared_ptr<RenderPass> createRenderPass(
        const RenderPassCreateInfo &info) = 0;
    // todo: cleaner interface
    virtual std::shared_ptr<Framebuffer> createFramebuffer(
        const RenderPassCreateInfo &info,
        const Vector2u32 &size,
        std::initializer_list<GpuImageView*> views) = 0;
    virtual std::shared_ptr<GpuSemaphore> createSemaphore() = 0;

    virtual void submitGraphicsJobs(
        std::vector<std::shared_ptr<GraphicsCommandList>> jobs,
        std::initializer_list<std::shared_ptr<GpuSemaphore>> wait_semaphores,
        std::initializer_list<GraphicsPipelineStage> wait_stages,
        std::initializer_list<std::shared_ptr<GpuSemaphore>> signal_semaphores
    ) = 0;

    /**
     * \brief Release resources used by previous jobs.
     */
    virtual void reclaimResources() = 0;
};
}
