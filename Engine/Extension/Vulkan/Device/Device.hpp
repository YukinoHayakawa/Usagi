#pragma once

#include <mutex>
#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/Device/Device.hpp>

namespace yuki::extension::vulkan
{
class Device : public graphics::Device
{
    vk::UniqueInstance mInstance;
    vk::UniqueDebugReportCallbackEXT mDebugReportCallback;
    vk::PhysicalDevice mPhysicalDevice;
    vk::UniqueDevice mDevice;
    // since Event must be used in the same queue, this queue processes
    // both graphics and transfer commands.
    vk::Queue mGraphicsQueue;
    uint32_t mGraphicsQueueFamilyIndex = -1;
    std::mutex mSubmitMutex;

    static VKAPI_ATTR VkBool32 VKAPI_CALL _debugLayerCallback(
        VkDebugReportFlagsEXT flags,
        VkDebugReportObjectTypeEXT objType,
        uint64_t obj,
        size_t location,
        int32_t code,
        const char *layerPrefix,
        const char *msg,
        void *userData
    );

    void _createInstance();
    void _createDebugReport();
    void _selectPhysicalDevice();
    void createDeviceAndQueues();
    static void selectQueue(std::vector<vk::QueueFamilyProperties> &queue_family,
        const vk::QueueFlags &queue_flags, std::size_t *selected_index);

public:
    Device();
    ~Device() override;

    // todo: remove param
    std::unique_ptr<graphics::SwapChain> createSwapChain(Window *window) override;
    std::unique_ptr<graphics::Pipeline> createPipeline(
        const graphics::PipelineCreateInfo &info) override;
    std::unique_ptr<graphics::ResourceManager> createResourceManager() override;
    std::unique_ptr<graphics::CommandPool> createGraphicsCommandPool() override;
    std::unique_ptr<graphics::FrameController> createFrameController(size_t num_frames)
    override;
    //std::unique_ptr<graphics::Sampler> createSampler(
    //  const graphics::SamplerCreateInfo &info) override;

    void submitGraphicsCommandList(
        class graphics::CommandList *command_list,
        const std::vector<graphics::Waitable *> &device_wait_objects,
        const std::vector<graphics::Waitable *> &device_signal_objects,
        graphics::Waitable *host_sync_fence
    ) override;

    void waitIdle() override;

    uint32_t getGraphicsQueueFamilyIndex() const;
    // uint32_t getPresentQueueFamilyIndex() const;

    vk::Device device() const;
    vk::PhysicalDevice physicalDevice() const;
    vk::Instance _getInstance() const;
    vk::Queue _getGraphicsQueue() const;

    void submitCommandBuffer(vk::CommandBuffer command_buffer, vk::Fence fence);
};
}
