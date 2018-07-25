#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Graphics/GpuDevice.hpp>

namespace usagi
{
class VulkanGpuDevice : public GpuDevice
{
    vk::UniqueInstance mInstance;
    vk::UniqueDebugReportCallbackEXT mDebugReportCallback;
    vk::PhysicalDevice mPhysicalDevice;
    vk::UniqueDevice mDevice;

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
    );

    void createInstance();
    void createDebugReport();
    void selectPhysicalDevice();
    void createDeviceAndQueues();

public:
    VulkanGpuDevice();

    GpuImage * swapChainImage() override;
    std::unique_ptr<GraphicsPipelineCompiler> createPipelineCompiler() override;

    std::shared_ptr<SwapChain> createSwapChain(Window *window) override;

    vk::Device device();
    vk::PhysicalDevice physicalDevice();
};
}
