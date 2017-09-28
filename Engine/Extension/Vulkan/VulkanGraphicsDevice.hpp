#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

namespace yuki
{

class VulkanGraphicsDevice : public std::enable_shared_from_this<VulkanGraphicsDevice>
{
    friend class VulkanSwapChain;

    vk::Instance mInstance;
    vk::DebugReportCallbackEXT mDebugReportCallback;
    vk::PhysicalDevice mPhysicalDevice;
    vk::Device mDevice;
    vk::Queue mGraphicsQueue;

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
    virtual ~VulkanGraphicsDevice();

    virtual std::shared_ptr<class SwapChain> createSwapChain(std::shared_ptr<class Window> window);
};

}
