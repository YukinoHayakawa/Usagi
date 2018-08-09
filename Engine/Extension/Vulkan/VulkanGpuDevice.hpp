#pragma once

#include <deque>

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/GpuDevice.hpp>

namespace usagi
{
class VulkanBatchResource;

class VulkanGpuDevice : public GpuDevice
{
    vk::UniqueInstance mInstance;
    vk::UniqueDebugReportCallbackEXT mDebugReportCallback;
    vk::PhysicalDevice mPhysicalDevice;
    vk::UniqueDevice mDevice;

    vk::Queue mGraphicsQueue;
    std::uint32_t mGraphicsQueueFamilyIndex = -1;

    struct BatchResourceList
    {
        vk::UniqueFence fence;
        std::vector<std::shared_ptr<VulkanBatchResource>> resources;
    };
    std::deque<BatchResourceList> mBatchResourceLists;

    static uint32_t selectQueue(
        std::vector<vk::QueueFamilyProperties> &queue_family,
        const vk::QueueFlags &queue_flags);
    void checkQueuePresentationCapacity(uint32_t queue_family_index) const;

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugLayerCallbackDispatcher(
        VkDebugReportFlagsEXT flags,
        VkDebugReportObjectTypeEXT objType,
        uint64_t obj,
        size_t location,
        int32_t code,
        const char *layerPrefix,
        const char *msg,
        void *userData
    );

    bool debugLayerCallback(
        vk::DebugReportFlagsEXT flags,
        vk::DebugReportObjectTypeEXT obj_type,
        std::uint64_t obj,
        std::size_t location,
        std::int32_t code,
        const char *layer_prefix,
        const char *msg
    ) const;

    void createInstance();
    void createDebugReport();
    void selectPhysicalDevice();
    void createDeviceAndQueues();

public:
    VulkanGpuDevice();
    ~VulkanGpuDevice();

    std::unique_ptr<GraphicsPipelineCompiler> createPipelineCompiler() override;
    std::shared_ptr<Swapchain> createSwapchain(Window *window) override;
    std::unique_ptr<GpuCommandPool> createCommandPool() override;
    std::shared_ptr<RenderPass> createRenderPass(
        const RenderPassCreateInfo &info) override;
    std::shared_ptr<Framebuffer> createFramebuffer(
        const RenderPassCreateInfo &info,
        const Vector2u32 &size,
        std::initializer_list<GpuImageView *> views) override;
    std::shared_ptr<GpuSemaphore> createSemaphore() override;

    void submitGraphicsJobs(
        std::vector<std::shared_ptr<GraphicsCommandList>> jobs,
        std::initializer_list<std::shared_ptr<GpuSemaphore>> wait_semaphores,
        std::initializer_list<GraphicsPipelineStage> wait_stages,
        std::initializer_list<std::shared_ptr<GpuSemaphore>> signal_semaphores
    ) override;

    void reclaimResources() override;

    vk::Device device() const;
    vk::PhysicalDevice physicalDevice() const;
    uint32_t graphicsQueueFamily() const;

    vk::Queue presentQueue() const;
};
}
