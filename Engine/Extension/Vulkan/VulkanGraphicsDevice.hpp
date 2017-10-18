#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/GraphicsDevice2.hpp>

namespace yuki
{

class VulkanGraphicsDevice : public GraphicsDevice2
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
    VulkanGraphicsDevice();
    ~VulkanGraphicsDevice() override;

    // todo: remove param
    std::unique_ptr<SwapChain> createSwapChain(Window *window) override;
    std::unique_ptr<GraphicsPipeline> createGraphicsPipeline() override;
    std::unique_ptr<GraphicsCommandPool> createGraphicsCommandPool() override;
    std::unique_ptr<FrameController> createFrameController(size_t num_frames) override;

    void submitGraphicsCommandList(
        class GraphicsCommandList *command_list,
        const std::vector<GraphicsSemaphore *> &wait_semaphores,
        const std::vector<GraphicsSemaphore *> &signal_semaphores
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
