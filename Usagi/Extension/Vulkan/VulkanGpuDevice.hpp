#pragma once

#include <deque>

#include <vulkan/vulkan.hpp>

#include <Usagi/Runtime/Graphics/GpuDevice.hpp>

#include "VulkanMemoryPool.hpp"

namespace usagi
{
class BitmapMemoryAllocator;
class VulkanMemoryPool;
class VulkanBatchResource;

class VulkanGpuDevice : public GpuDevice
{
    vk::UniqueInstance mInstance;
    vk::UniqueDebugUtilsMessengerEXT mDebugUtilsMessenger;
    vk::PhysicalDevice mPhysicalDevice;
    vk::UniqueDevice mDevice;

    static void addPlatformSurfaceExtension(
        std::vector<const char *> & extensions);

    vk::Queue mGraphicsQueue;
    std::uint32_t mGraphicsQueueFamilyIndex = -1;

    static uint32_t selectQueue(
        std::vector<vk::QueueFamilyProperties> &queue_family,
        const vk::QueueFlags &queue_flags);
    void checkQueuePresentationCapacity(uint32_t queue_family_index) const;

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugMessengerCallbackDispatcher(
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT message_type,
        const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
        void *user_data);

    VkBool32 debugMessengerCallback(
        const vk::DebugUtilsMessageSeverityFlagsEXT &message_severity,
        const vk::DebugUtilsMessageTypeFlagsEXT &message_type,
        const vk::DebugUtilsMessengerCallbackDataEXT *callback_data) const;

    void createInstance();
    void createDebugReport();
    void selectPhysicalDevice();
    void createDeviceAndQueues();

    // Memory Management

    using BitmapBufferPool = VulkanBufferMemoryPool<BitmapMemoryAllocator>;
    /**
     * \brief Used for per-frame updated buffers and resource staging.
     */
    std::unique_ptr<BitmapBufferPool> mDynamicBufferPool;

    using BitmapImagePool = VulkanImageMemoryPool<BitmapMemoryAllocator>;
    /**
     * \brief Used for device-local textures.
     */
    std::unique_ptr<BitmapImagePool> mDeviceImagePool;

    vk::UniqueCommandPool mTransferCommandPool;

    void createMemoryPools();

    // Resource Tracking

    struct BatchResourceList
    {
        vk::UniqueFence fence;
        std::vector<std::shared_ptr<VulkanBatchResource>> resources;
    };
    // must be the first to be destructed in dtor since it may refer to other
    // members.
    std::deque<BatchResourceList> mBatchResourceLists;

public:
    VulkanGpuDevice();
    ~VulkanGpuDevice();

    std::unique_ptr<GraphicsPipelineCompiler> createPipelineCompiler() override;
    std::shared_ptr<Swapchain> createSwapchain(Window *window) override;
    std::shared_ptr<GpuCommandPool> createCommandPool() override;
    std::shared_ptr<RenderPass> createRenderPass(
        const RenderPassCreateInfo &info) override;
    std::shared_ptr<Framebuffer> createFramebuffer(
        const Vector2u32 &size,
        std::initializer_list<std::shared_ptr<GpuImageView>> views) override;
    std::shared_ptr<GpuSemaphore> createSemaphore() override;
    std::shared_ptr<GpuBuffer> createBuffer(GpuBufferUsage usage) override;
    std::shared_ptr<GpuImage> createImage(const GpuImageCreateInfo &info)
        override;
    std::shared_ptr<GpuSampler> createSampler(const GpuSamplerCreateInfo &info)
        override;

    void submitGraphicsJobs(
        std::vector<std::shared_ptr<GraphicsCommandList>> jobs,
        std::initializer_list<std::shared_ptr<GpuSemaphore>> wait_semaphores,
        std::initializer_list<GraphicsPipelineStage> wait_stages,
        std::initializer_list<std::shared_ptr<GpuSemaphore>> signal_semaphores
    ) override;

    void reclaimResources() override;
    void waitIdle() override;

    vk::Device device() const;
    vk::PhysicalDevice physicalDevice() const;
    uint32_t graphicsQueueFamily() const;

    vk::Queue presentQueue() const;

    std::shared_ptr<VulkanBufferAllocation> allocateStageBuffer(
        std::size_t size);
    void copyBufferToImage(
        const std::shared_ptr<VulkanBufferAllocation> &buffer,
        VulkanGpuImage *image);
};
}
