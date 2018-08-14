#pragma once

#include <memory>
#include <vector>

#include <Usagi/Engine/Utility/Noncopyable.hpp>
#include <Usagi/Engine/Core/Math.hpp>

#include "Enum/GpuBufferUsage.hpp"

namespace usagi
{
struct GpuImageCreateInfo;
class GpuSampler;
struct GpuSamplerCreateInfo;
class GpuBuffer;
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
    virtual std::shared_ptr<GpuCommandPool> createCommandPool() = 0;
    virtual std::unique_ptr<GraphicsPipelineCompiler>
        createPipelineCompiler() = 0;
    virtual std::shared_ptr<RenderPass> createRenderPass(
        const RenderPassCreateInfo &info) = 0;
    virtual std::shared_ptr<Framebuffer> createFramebuffer(
        const Vector2u32 &size,
        std::initializer_list<std::shared_ptr<GpuImageView>> views) = 0;
    virtual std::shared_ptr<GpuSemaphore> createSemaphore() = 0;
    virtual std::shared_ptr<GpuImage> createImage(
        const GpuImageCreateInfo &info) = 0;

    /**
     * \brief Memory is not allocated until GpuBuffer::allocate() is called.
     * \return
     */
    virtual std::shared_ptr<GpuBuffer> createBuffer(GpuBufferUsage usage) = 0;
    virtual std::shared_ptr<GpuSampler> createSampler(
        const GpuSamplerCreateInfo &info) = 0;

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
    virtual void waitIdle() = 0;
};
}
