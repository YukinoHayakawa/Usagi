#include <iostream>

#include <Usagi/Engine/Extension/Win32/Win32Window.hpp>

#include "VulkanGraphicsDevice.hpp"
#include "VulkanSwapChain.hpp"

VkBool32 yuki::VulkanGraphicsDevice::_debugLayerCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char *layerPrefix, const char *msg, void *userData)
{
    vk::DebugReportFlagsEXT level(static_cast<vk::DebugReportFlagBitsEXT>(flags));
    if(level & vk::DebugReportFlagBitsEXT::eInformation)
        std::cout << "[INFO] ";
    if(level & vk::DebugReportFlagBitsEXT::eWarning)
        std::cout << "[WARN] ";
    if(level & vk::DebugReportFlagBitsEXT::ePerformanceWarning)
        std::cout << "[PERF] ";
    if(level & vk::DebugReportFlagBitsEXT::eError)
        std::cout << "[ERROR] ";
    if(level & vk::DebugReportFlagBitsEXT::eDebug)
        std::cout << "[DEBUG] ";

    std::cout << msg << std::endl;

    return VK_FALSE;
}

void yuki::VulkanGraphicsDevice::_createInstance()
{
    vk::ApplicationInfo application_info;
    application_info.setPApplicationName("Usagi Engine World Editor");
    application_info.setApplicationVersion(VK_MAKE_VERSION(1, 0, 0));
    application_info.setPEngineName("Usagi Engine");
    application_info.setEngineVersion(VK_MAKE_VERSION(1, 0, 0));
    application_info.setApiVersion(VK_API_VERSION_1_0);

    std::cout << "Creating Vulkan intance" << std::endl;

    std::cout << "\nAvailable instance extensions:" << std::endl;
    std::cout << "==============================" << std::endl;
    for(auto &&ext : vk::enumerateInstanceExtensionProperties())
    {
        std::cout << "    " << ext.extensionName << std::endl;
    }

    std::cout << "\nAvailable validation layers:" << std::endl;
    std::cout << "============================" << std::endl;
    for(auto &&layer : vk::enumerateInstanceLayerProperties())
    {
        std::cout << "    " << layer.layerName << ": " << layer.description << std::endl;
    }

    vk::InstanceCreateInfo instance_create_info;
    instance_create_info.setPApplicationInfo(&application_info);

    const std::vector<const char *> instance_extensions
    {
        "VK_KHR_surface", // application window
        "VK_KHR_win32_surface", // todo remove OS dependency
        "VK_EXT_debug_report", // provide feedback from validation layer
    };
    instance_create_info.setEnabledExtensionCount(instance_extensions.size());
    instance_create_info.setPpEnabledExtensionNames(instance_extensions.data());

    // todo: enumerate layers first
    // todo: disable in release
    const std::vector<const char*> validationLayers
    {
        "VK_LAYER_LUNARG_standard_validation"
    };
    instance_create_info.setEnabledLayerCount(validationLayers.size());
    instance_create_info.setPpEnabledLayerNames(validationLayers.data());

    mInstance = vk::createInstance(instance_create_info);
}

void yuki::VulkanGraphicsDevice::_createDebugReport()
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
    debug_info.setPfnCallback(&_debugLayerCallback);
    mDebugReportCallback = mInstance.createDebugReportCallbackEXT(debug_info);
}

void yuki::VulkanGraphicsDevice::_selectPhysicalDevice()
{
    auto physical_devices = mInstance.enumeratePhysicalDevices();
    for(auto &&dev : physical_devices)
    {
        auto prop = dev.getProperties();
        std::cout
            << "\n  * Device Name:    " << prop.deviceName
            << "\n    Device Type:    " << to_string(prop.deviceType)
            << "\n    Device ID:      " << prop.deviceID
            << "\n    API Version:    " << prop.apiVersion
            << "\n    Vendor ID:      " << prop.vendorID
            << "\n    Driver Version: " << prop.driverVersion
            << "\n" << std::endl;

        // todo: select device based on features and score them / let the user choose
        if(!mPhysicalDevice)
            mPhysicalDevice = dev;
        else if(prop.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
            mPhysicalDevice = dev;
    }
    if(!mPhysicalDevice)
        throw std::runtime_error("no suitable Vulkan device was found!");
    std::cout << "Using device: " << mPhysicalDevice.getProperties().deviceName << std::endl;
}

void yuki::VulkanGraphicsDevice::_createGraphicsQueue()
{
    std::cout << "\nSupported queue families:" << std::endl;
    std::cout << "=========================" << std::endl;
    auto queue_family = mPhysicalDevice.getQueueFamilyProperties();
    auto graphics_queue_index = queue_family.end();
    for(auto iter = queue_family.begin(); iter != queue_family.end(); ++iter)
    {
        std::cout << "    (" << iter->queueCount << ") " << to_string(iter->queueFlags) << std::endl;
        if(iter->queueFlags & vk::QueueFlagBits::eGraphics)
            graphics_queue_index = iter;

    }
    if(graphics_queue_index == queue_family.end())
        throw std::runtime_error("no queue family supporting graphics operations was found");

    vk::DeviceCreateInfo device_create_info;

    vk::DeviceQueueCreateInfo queue_create_info;
    queue_create_info.setQueueFamilyIndex(graphics_queue_index - queue_family.begin());
    queue_create_info.setQueueCount(1);
    float queue_priority = 1;
    queue_create_info.setPQueuePriorities(&queue_priority);
    device_create_info.setQueueCreateInfoCount(1);
    device_create_info.setPQueueCreateInfos(&queue_create_info);

    // todo: check device capacity
    const std::vector<const char *> device_extensions
    {
        "VK_KHR_swapchain",
    };
    device_create_info.setEnabledExtensionCount(device_extensions.size());
    device_create_info.setPpEnabledExtensionNames(device_extensions.data());

    // note that device layers are deprecated

    vk::PhysicalDeviceFeatures physical_device_features;
    device_create_info.setPEnabledFeatures(&physical_device_features);

    mDevice = mPhysicalDevice.createDevice(device_create_info);
    mGraphicsQueue = mDevice.getQueue(queue_create_info.queueFamilyIndex, 0);
}

yuki::VulkanGraphicsDevice::VulkanGraphicsDevice()
{
    _createInstance();
    _createDebugReport();
    _selectPhysicalDevice();
    _createGraphicsQueue();
}

yuki::VulkanGraphicsDevice::~VulkanGraphicsDevice()
{
    mDevice.destroy();
    mInstance.destroyDebugReportCallbackEXT(mDebugReportCallback);
    mInstance.destroy();
}

std::shared_ptr<yuki::SwapChain> yuki::VulkanGraphicsDevice::createSwapChain(std::shared_ptr<Window> window)
{
    if(auto native_window = std::dynamic_pointer_cast<Win32Window>(window))
    {
        return std::make_shared<VulkanSwapChain>(shared_from_this(), native_window->getProcessInstanceHandle(), native_window->getNativeWindowHandle());
    }
    throw std::runtime_error("unsupported window system");
}
