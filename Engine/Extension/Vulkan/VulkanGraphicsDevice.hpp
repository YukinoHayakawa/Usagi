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
    std::shared_ptr<SwapChain> createSwapChain(std::shared_ptr<class Window> window) override;
    std::shared_ptr<GraphicsPipeline> createGraphicsPipeline() override;
    std::shared_ptr<GraphicsCommandPool> createGraphicsCommandPool() override;
    std::shared_ptr<VertexBuffer> createVertexBuffer(size_t size) override;

    void submitGraphicsCommandList(
        class GraphicsCommandList *command_list,
        const std::vector<const GraphicsSemaphore *> &wait_semaphores,
        const std::vector<const GraphicsSemaphore *> &signal_semaphores
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
