#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/Device/Device.hpp>

namespace yuki::vulkan
{

class Device : public graphics::Device
{
    vk::UniqueInstance mInstance;
    vk::UniqueDebugReportCallbackEXT mDebugReportCallback;
    vk::PhysicalDevice mPhysicalDevice;
    vk::UniqueDevice mDevice;
    vk::Queue mGraphicsQueue;
    uint32_t mGraphicsQueueFamilyIndex = -1;

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
    void _createGraphicsQueue();

public:
    Device();
    ~Device() override;

    // todo: remove param
    std::unique_ptr<graphics::SwapChain> createSwapChain(Window *window) override;
    std::unique_ptr<graphics::Pipeline> createPipeline() override;
    std::unique_ptr<graphics::CommandPool> createGraphicsCommandPool() override;
    std::unique_ptr<graphics::FrameController> createFrameController(size_t num_frames) override;

    void submitGraphicsCommandList(
        class graphics::CommandList *command_list,
        const std::vector<graphics::Waitable *> &device_wait_objects,
        const std::vector<graphics::Waitable *> &device_signal_objects,
        graphics::Waitable *host_sync_fence
    ) override;

    void waitIdle() override;

    uint32_t getGraphicsQueueFamilyIndex() const;
    // uint32_t getPresentQueueFamilyIndex() const;

    vk::Device _getDevice() const;
    vk::PhysicalDevice _getPhysicalDevice() const;
    vk::Instance _getInstance() const;
    vk::Queue _getGraphicsQueue() const;
};

}
