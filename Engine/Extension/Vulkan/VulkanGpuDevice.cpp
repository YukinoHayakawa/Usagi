#include "VulkanGpuDevice.hpp"

#include <loguru.hpp>

VkBool32 usagi::VulkanGpuDevice::debugLayerCallbackDispatcher(
    VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT obj_type,
    uint64_t obj, size_t location, int32_t code,
    const char *layer_prefix, const char *msg, void *user_data)
{
    auto device = reinterpret_cast<VulkanGpuDevice*>(user_data);
    return device->debugLayerCallback(
        static_cast<vk::DebugReportFlagBitsEXT>(flags),
        static_cast<vk::DebugReportObjectTypeEXT>(obj_type),
        obj,
        location,
        code,
        layer_prefix,
        msg
    );
}

bool usagi::VulkanGpuDevice::debugLayerCallback(
    vk::DebugReportFlagsEXT flags,
    vk::DebugReportObjectTypeEXT obj_type,
    std::uint64_t obj,
    std::size_t location,
    std::int32_t code,
    const char *layer_prefix,
    const char *msg)
{
    if(flags & vk::DebugReportFlagBitsEXT::eInformation)
    {
        LOG_F(INFO, msg);
    }
    if(flags & vk::DebugReportFlagBitsEXT::eWarning)
    {
        LOG_F(WARNING, msg);
    }
    if(flags & vk::DebugReportFlagBitsEXT::ePerformanceWarning)
    {
        LOG_F(WARNING, "(Perf) %s", msg);
    }
    if(flags & vk::DebugReportFlagBitsEXT::eError)
    {
        LOG_F(ERROR, msg);
    }
    if(flags & vk::DebugReportFlagBitsEXT::eDebug)
    {
        //LOG(DEBUG) << msg;
    }
    return false;
}

void usagi::VulkanGpuDevice::createInstance()
{
    LOG_F(INFO, "Creating Vulkan intance");

    vk::ApplicationInfo application_info;
    application_info.setPApplicationName("UsagiEngine");
    application_info.setApplicationVersion(VK_MAKE_VERSION(1, 0, 0));
    application_info.setPEngineName("Usagi");
    application_info.setEngineVersion(VK_MAKE_VERSION(1, 0, 0));
    application_info.setApiVersion(VK_API_VERSION_1_0);

    // Extensions
    {
        LOG_SCOPE_F(INFO, "Available instance extensions");
        for(auto &&ext : vk::enumerateInstanceExtensionProperties())
        {
            LOG_F(INFO, ext.extensionName);
        }
    }
    // Validation layers
    {
        LOG_SCOPE_F(INFO, "Available validation layers");
        for(auto &&layer : vk::enumerateInstanceLayerProperties())
        {
            LOG_F(INFO, "Name:        %s", layer.layerName);
            LOG_F(INFO, "Description: %s", layer.description);
        }
    }

    vk::InstanceCreateInfo instance_create_info;
    instance_create_info.setPApplicationInfo(&application_info);

    const std::vector<const char *> instance_extensions
    {
        // application window
        "VK_KHR_surface",
        // todo remove OS dependency
        "VK_KHR_win32_surface",
        // provide feedback from validation layer
        "VK_EXT_debug_report",
    };
    instance_create_info.setEnabledExtensionCount(
        static_cast<uint32_t>(instance_extensions.size()));
    instance_create_info.setPpEnabledExtensionNames(instance_extensions.data());

    // todo: enumerate layers first
    // todo: disable in release
    const std::vector<const char*> validationLayers
    {
        "VK_LAYER_LUNARG_standard_validation"
    };
    instance_create_info.setEnabledLayerCount(
        static_cast<uint32_t>(validationLayers.size()));
    instance_create_info.setPpEnabledLayerNames(validationLayers.data());

    mInstance = vk::createInstanceUnique(instance_create_info);
}

void usagi::VulkanGpuDevice::createDebugReport()
{
    vk::DebugReportCallbackCreateInfoEXT debug_info;
    debug_info.setFlags(
        vk::DebugReportFlagBitsEXT::eInformation |
        vk::DebugReportFlagBitsEXT::eWarning |
        vk::DebugReportFlagBitsEXT::ePerformanceWarning |
        vk::DebugReportFlagBitsEXT::eError |
        vk::DebugReportFlagBitsEXT::eDebug |
        vk::DebugReportFlagBitsEXT(0)
    );
    debug_info.setPfnCallback(&debugLayerCallbackDispatcher);
    debug_info.pUserData = this;
    mDebugReportCallback =
        mInstance->createDebugReportCallbackEXTUnique(debug_info);
}

void usagi::VulkanGpuDevice::selectPhysicalDevice()
{
    {
        LOG_SCOPE_F(INFO, "Available physical devices");
        auto physical_devices = mInstance->enumeratePhysicalDevices();
        for(auto &&dev : physical_devices)
        {
            const auto prop = dev.getProperties();
            LOG_S(INFO) << "Device Name:    " << prop.deviceName;
            LOG_S(INFO) << "Device Type:    " << to_string(prop.deviceType);
            LOG_S(INFO) << "Device ID:      " << prop.deviceID;
            LOG_S(INFO) << "API Version:    " << prop.apiVersion;
            LOG_S(INFO) << "Driver Version: " << prop.vendorID;
            LOG_S(INFO) << "Vendor ID:      " << prop.vendorID;
            // todo: select device based on features and score them / let the
            // user choose
            if(!mPhysicalDevice)
                mPhysicalDevice = dev;
            else if(prop.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
                mPhysicalDevice = dev;
        }
    }
    if(!mPhysicalDevice)
        throw std::runtime_error("No available GPU supporting Vulkan.");
    LOG_S(INFO) << "Using physical device: "
        << mPhysicalDevice.getProperties().deviceName;
}

usagi::VulkanGpuDevice::VulkanGpuDevice()
{
    createInstance();
    createDebugReport();
    selectPhysicalDevice();
}

usagi::GpuImage * usagi::VulkanGpuDevice::swapChainImage()
{
    throw 0;
}

std::unique_ptr<usagi::GraphicsPipelineCompiler> usagi::VulkanGpuDevice::
    createPipelineCompiler()
{
    throw 0;
}

vk::Device usagi::VulkanGpuDevice::device()
{
    throw 0;
}
